// (c) 2019 Beem Media. All Rights Reserved.

#include "EGLoader.h"
#include "DeMenu.h"
#include "EGList.h"
#include "EGLoader_Loadable.h"
#include "EGEngineConfig.h"
#include "DeCreditsData.h"
#include "EGFileData.h"
#include "EGUiWidget.h"

static const eg_real CREDITS_MENU_HEADER_POP_TIME = 7.f;

enum class de_credit_node_t : eg_uint
{
	NONE,
	LEFT,
	RIGHT,
	CENTER,
	HEADER,
	BLOB,
	FUTURE_POP,
	FUTURE_HEADER_POP,
	HEADER_POP,

	COUNT,
};


static const struct exCreditTypeInfo
{
	de_credits_line_t Type;
	eg_bool           bPop:1;
	de_credit_node_t  Node1Type;
	eg_color32        Node1Color;
	de_credit_node_t  Node2Type;
	eg_color32        Node2Color;
}
CreditsMenuTable[] =
{
	{ de_credits_line_t::Delay           , false , de_credit_node_t::NONE              , eg_color32(255,255,255) ,  de_credit_node_t::NONE  , eg_color32(255,255,255) } ,
	{ de_credits_line_t::Normal          , false , de_credit_node_t::LEFT              , eg_color32(255,255,255) ,  de_credit_node_t::RIGHT , eg_color32(255,255,255) } ,
	{ de_credits_line_t::Header          , false , de_credit_node_t::HEADER            , eg_color32(255,255,255) ,  de_credit_node_t::NONE  , eg_color32(255,255,255) } ,
	{ de_credits_line_t::Center          , false , de_credit_node_t::CENTER            , eg_color32(252,255,255) ,  de_credit_node_t::NONE  , eg_color32(255,255,255) } ,
	{ de_credits_line_t::HeaderPop       , true  , de_credit_node_t::HEADER_POP        , eg_color32(255,255,255) ,  de_credit_node_t::NONE  , eg_color32(255,255,255) } ,
	{ de_credits_line_t::FutureHeaderPop , true  , de_credit_node_t::FUTURE_HEADER_POP , eg_color32(255,255,255) ,  de_credit_node_t::NONE  , eg_color32(255,255,255) } ,
	{ de_credits_line_t::Blob            , false , de_credit_node_t::BLOB              , eg_color32(255,255,255) ,  de_credit_node_t::NONE  , eg_color32(255,255,255) } ,
	{ de_credits_line_t::FuturePop       , true  , de_credit_node_t::FUTURE_POP        , eg_color32(255,255,255) ,  de_credit_node_t::NONE  , eg_color32(255,255,255) } ,

};

const exCreditTypeInfo& ExCreditsMenu_GetInfoForType( de_credits_line_t Type )
{
	for( eg_size_t i=0; i<countof(CreditsMenuTable); i++ )
	{
		if( CreditsMenuTable[i].Type == Type )
		{
			return CreditsMenuTable[i];
		}
	}

	assert( false );
	return CreditsMenuTable[0];
};

class DeCreditsMenu: public DeMenu , public ILoadable
{
	EG_CLASS_BODY( DeCreditsMenu , DeMenu )

private:
		
	struct exCreditNode: public IListable
	{
		EGUiWidget*      TextNode = nullptr;
		eg_loc_text       LocText = eg_loc_text( eg_crc("") );
		DeCreditsMenu*    OwnerMenu = nullptr;
		eg_color32        Color = eg_color32(255,255,255);
		de_credit_node_t  Type = de_credit_node_t::NONE;
		eg_real           LifeSeconds = 0.f;
		eg_bool           bInMotion = false;
		eg_bool           bPop = false;

		void Update( eg_real DeltaTime )
		{
			if( bInMotion )
			{
				if( bPop )
				{
					TextNode->SetOffset( EGUiWidget::eg_offset_t::POST , eg_transform( CT_Default ) );
					eg_real Alpha = 3.f*EGMath_CubicInterp( 0.f, 0.f, 1.f, 0.f, EGMath_GetMappedRangeValue( LifeSeconds , eg_vec2( 0.f, CREDITS_MENU_HEADER_POP_TIME ), eg_vec2( 0.f, 1.f ) ) );
					
					if( Alpha > 1.f )
					{
						if( Alpha > 2.f )
						{
							Alpha = 3.f - Alpha;
						}
						else
						{
							Alpha = 1.f;
						}
					}

					eg_color ColorVec( Color );
					ColorVec.a = Alpha;

					TextNode->SetPalette( 0 , ColorVec.ToVec4() );

					if( LifeSeconds > CREDITS_MENU_HEADER_POP_TIME )
					{
						OwnerMenu->FreeNode( this );
					}
				}
				else
				{
					eg_transform StartPose;
					eg_transform EndPose;
					StartPose = eg_transform::BuildTranslation( 0.f , -120.f , 0.f );
					EndPose = eg_transform::BuildTranslation( 0.f , 130.f , 0.f );

					static const eg_real DURATION = 16.f;

					if( LifeSeconds >= DURATION )
					{
						OwnerMenu->FreeNode( this );
					}
					else
					{
						eg_real BlendPct = EGMath_GetMappedRangeValue( LifeSeconds , eg_vec2( 0.f, DURATION ), eg_vec2( 0.f, 1.f ) );

						eg_transform FinalPose = eg_transform::Lerp( StartPose , EndPose , BlendPct );

						TextNode->SetOffset( EGUiWidget::eg_offset_t::POST , FinalPose );
					}
				}
				LifeSeconds += DeltaTime;
			}
		}
	};

private:

	EGArray<deCreditsLine> m_Credits = eg_mem_pool::DefaultHi; // The credits will get populated from another thread, but we're not going to use mutexes to do it because we know once the data is valid.
	EGArray<exCreditNode>  m_MasterNodeList; // Only holds the instances, does not manage anything.
	EGList<exCreditNode>   m_FreeNodes[de_credit_node_t::COUNT];

	eg_size_t              m_NextCreditEvent;
	eg_real                m_TimeTillNextCreditEvent;
	eg_real                m_CreditsSpeed = 1.f;
	eg_bool                m_bCreditsReady;
	eg_bool                m_bSaveCredits = false;

public:
	DeCreditsMenu()
	: DeMenu()
	{ 
		for( eg_size_t i=0; i<countof(m_FreeNodes); i++ )
		{
			m_FreeNodes[i].Init( EGList<exCreditNode>::DEFAULT_ID ); // Not shared os all can be initialized with the same id.
		}
	}

	void FreeNode( exCreditNode* Node )
	{
		assert( Node->bInMotion );
		Node->bInMotion = false;
		Node->TextNode->SetVisible( false );
		if( Node )
		{
			switch( Node->Type )
			{
			case de_credit_node_t::COUNT:
			case de_credit_node_t::NONE:
				assert( false );
				break;
			default:
				m_FreeNodes[static_cast<eg_uint>(Node->Type)].Insert( Node );
				break;
			}
		}
	}

	void Init_TextNodes()
	{
		m_MasterNodeList.Reserve( 22*3 + 5 + 20 ); // Might as well avoid allocations since we know about how many there are.

		auto StoreTextNode = [this]( EGUiWidget* TextNode , de_credit_node_t Type , eg_size_t Extras ) -> void
		{
			if( TextNode )
			{
				exCreditNode NewNode;
				NewNode.OwnerMenu = this;
				NewNode.Type = Type;
				NewNode.TextNode = TextNode;
				NewNode.TextNode->SetVisible( false );
				m_MasterNodeList.Append( NewNode );

				for( eg_size_t i=0; i<Extras; i++ )
				{
					EGUiWidget* Duplicate = DuplicateWidget( TextNode );
					if( Duplicate )
					{
						exCreditNode NewNode;
						NewNode.OwnerMenu = this;
						NewNode.Type = Type;
						NewNode.TextNode = Duplicate;
						NewNode.TextNode->SetVisible( false );
						m_MasterNodeList.Append( NewNode );
					}
				}
			}
		};

		EGUiWidget* Left    = GetWidget( eg_string_crc("Left"));
		EGUiWidget* Right   = GetWidget( eg_string_crc("Right"));
		EGUiWidget* Center  = GetWidget( eg_string_crc("Center"));
		EGUiWidget* Header  = GetWidget( eg_string_crc("Header"));
		EGUiWidget* Blob    = GetWidget( eg_string_crc("Blob"));
		EGUiWidget* FutureH = GetWidget( eg_string_crc("FutureHeaderPop"));
		EGUiWidget* HPop    = GetWidget( eg_string_crc("HeaderPop"));
		EGUiWidget* FutureP = GetWidget( eg_string_crc("FuturePop"));

		StoreTextNode( Left , de_credit_node_t::LEFT , 21 );
		StoreTextNode( Right , de_credit_node_t::RIGHT , 21 );
		StoreTextNode( Center , de_credit_node_t::CENTER , 21 );
		StoreTextNode( Header , de_credit_node_t::HEADER , 4 );
		StoreTextNode( Blob , de_credit_node_t::BLOB , 8 );
		StoreTextNode( FutureH , de_credit_node_t::FUTURE_HEADER_POP , 2 );
		StoreTextNode( HPop , de_credit_node_t::HEADER_POP , 2 );
		StoreTextNode( FutureP , de_credit_node_t::FUTURE_POP , 2 );


		//
		// We now have the master list so the pointers to the items in it won't change,
		// so we can populate the type lists.
		//
		for( exCreditNode& Node : m_MasterNodeList )
		{
			Node.bInMotion = true;
			FreeNode( &Node );
		}
	}

	void AddCreditToScreen( const deCreditsLine& Credit )
	{
		auto AddLocalizedNode = [&Credit]( EGList<exCreditNode>& List , eg_bool bIsLocText , const eg_color32& Color , eg_bool bPop ) -> void
		{
			exCreditNode* Node = List.GetOne(); // If this asserts we didn't have enough nodes on the stage.
			if( Node )
			{
				List.Remove( Node );
				assert( !Node->bInMotion );
				Node->bInMotion = true;
				Node->LocText = bIsLocText ? eg_loc_text(Credit.Title.Name) : eg_loc_text(Credit.Name.Name);
				Node->LifeSeconds = 0.f;
				Node->TextNode->SetVisible( true );
				Node->Color = Color;
				Node->TextNode->SetPalette( 0 , eg_color(Color).ToVec4() );
				Node->TextNode->SetText( eg_crc("") , Node->LocText );
				Node->bPop = bPop;
				Node->Update( 0.f ); // This will make it so it doesn't flash on in the wrong position when first created.
			}
		};

		const exCreditTypeInfo& Info = ExCreditsMenu_GetInfoForType( Credit.Type );

		eg_bool bIsLocText = true;

		if( Info.Node2Type != de_credit_node_t::NONE )
		{
			bIsLocText = true; // Node 1 will be a localized title.
			AddLocalizedNode( m_FreeNodes[static_cast<eg_uint>(Info.Node2Type)] , false , Info.Node2Color , Info.bPop );
		}
		else
		{
			bIsLocText = Credit.Title.Name != eg_crc("");
		}

		if( Info.Node1Type != de_credit_node_t::NONE )
		{
			AddLocalizedNode( m_FreeNodes[static_cast<eg_uint>(Info.Node1Type)] , bIsLocText , Info.Node1Color , Info.bPop );
		}
	}

	virtual void OnInit() override final
	{	
		if( m_bSaveCredits )
		{
			EGCrcDb::Init();
		}

		Super::OnInit();		
		assert( !m_bCreditsReady );
		m_bCreditsReady = false;
		MainLoader->BeginLoad( GAME_DATA_PATH "/gamedata/Credits.egasset" , this , EGLoader::LOAD_THREAD_MAIN );

		Init_TextNodes();

	}

	virtual void OnDeinit() override final
	{
		Super::OnDeinit();
		if( !m_bCreditsReady )
		{
			MainLoader->CancelLoad( this );
		}

		if( m_bSaveCredits )
		{
			EGCrcDb::Deinit();
		}
	}

	virtual void OnUpdate( eg_real DeltaTime , eg_real AspectRatio ) override final
	{
		Super::OnUpdate( DeltaTime , AspectRatio );

		DeltaTime *= m_CreditsSpeed;

		for( exCreditNode& Node : m_MasterNodeList )
		{
			Node.Update( DeltaTime );
		}

		if( m_bCreditsReady )
		{
			m_TimeTillNextCreditEvent -= DeltaTime;
			if( m_TimeTillNextCreditEvent < 0.f )
			{
				if( m_Credits.IsValidIndex( m_NextCreditEvent ) )
				{
					const deCreditsLine& Credit = m_Credits[m_NextCreditEvent];
					m_TimeTillNextCreditEvent = Credit.DelayTime;
					AddCreditToScreen( Credit );
					m_NextCreditEvent++;
				}
				else
				{
					m_NextCreditEvent = 0;
					MenuStack_Pop();
				}
			}
		}

	}

	virtual eg_bool OnInput( eg_menuinput_t InputType ) override final
	{
		if( InputType == eg_menuinput_t::BUTTON_BACK )
		{
			MenuStack_Pop();
			return true;
		}

		return false;
	}

	//
	// ILoadable Interface
	//

	virtual void DoLoad(eg_cpstr strFile, const eg_byte*const  pMem, const eg_size_t Size) override final
	{
		EGFileData Data( eg_file_data_init_t::SetableUserPointer );
		Data.SetData( pMem , Size );
		// eg_uint64 DeserializeStartTime = Timer_GetRawTime();
		DeCreditsData* CreditsData = EGDataAsset::LoadDataAsset<DeCreditsData>( Data , EGString_ToWide(strFile) );
		// eg_uint64 DeserializeEndTime = Timer_GetRawTime();
		// eg_real DeserializeSeconds = Timer_GetRawTimeElapsedSec( DeserializeStartTime , DeserializeEndTime );
		// EGLogf( eg_log_t::Warning , "Took %g seconds to deserialize." , DeserializeSeconds );
		if( CreditsData )
		{
			m_Credits = std::move( CreditsData->m_Lines );
			EGDeleteObject( CreditsData );
		}

		for( deCreditsLine& Line : m_Credits )
		{
			switch( Line.Type )
			{
				default:
					Line.DelayTime = 1.f;
					break;
				case de_credits_line_t::Blob:
					Line.DelayTime = 2.3f;
					break;
				case de_credits_line_t::HeaderPop:
				case de_credits_line_t::FuturePop:
				case de_credits_line_t::FutureHeaderPop:
				case de_credits_line_t::Delay:
					break;
			}
		}
	}

	virtual void OnLoadComplete(eg_cpstr strFile) override final
	{
		unused( strFile );
		m_bCreditsReady = true;

		if( m_bSaveCredits )
		{
			// SaveCredits();
		}

	}

	/*
	void SaveCredits()
	{
		ExCreditsData* CreditConv = EGCast<ExCreditsData>(EGDataAsset::CreateDataAsset( &ExCreditsData::GetStaticClass() , eg_mem_pool::DefaultHi , egRflEditor() ));
		if( CreditConv )
		{
			CreditConv->m_Lines.Resize( m_Credits.Len() );

			for( eg_size_t i=0; i<m_Credits.Len(); i++ )
			{
				exCreditsLine& Target = CreditConv->m_Lines[i];
				const exCredit& Source = m_Credits[i];

				Target.Name.Name = EGCrcDb::StringToCrc( EGString_ToMultibyte(Source.Name) );
				Target.Name.Name_enus = EGString_ToMultibyte(Source.Name);
				Target.Title.Name_enus = eg_string_big(EGString_ToMultibyte( eg_loc_text( Source.Title ).GetString() )).String();
				Target.Title.Name = Source.Title;
				Target.DelayTime = Source.DelayTillNext;

				switch( Source.Type )
				{
				case de_credit_t::DELAY:
					Target.Type = de_credits_line_t::Delay;
					Target.Comment = EGCrcDb::StringToCrc( "Delay" );
					break;
				case de_credit_t::PAIR:
					Target.Type = de_credits_line_t::Normal;
					Target.Comment = EGCrcDb::StringToCrc( "Credit" );
					break;
				case de_credit_t::HEADER:
					Target.Type = de_credits_line_t::Header;
					Target.Comment = EGCrcDb::StringToCrc( "Header" );
					break;
				case de_credit_t::CENTER:
					Target.Type = de_credits_line_t::Center;
					Target.Comment = EGCrcDb::StringToCrc( "Center Credit" );
					break;
				case de_credit_t::FUTURE_POP:
					Target.Type = de_credits_line_t::FuturePop;	
					Target.Comment = EGCrcDb::StringToCrc( "Future Pop Credit" );
					break;
				case de_credit_t::HEADER_POP:
					Target.Type = de_credits_line_t::HeaderPop;
					Target.Comment = EGCrcDb::StringToCrc( "Header Pop Credit" );
					break;
				case de_credit_t::FUTURE_HEADER_POP:
					Target.Type = de_credits_line_t::FutureHeaderPop;
					Target.Comment = EGCrcDb::StringToCrc( "Future Header Pop Credit" );
					break;
				case de_credit_t::BLOB:
					Target.Type = de_credits_line_t::Blob;
					Target.Comment = EGCrcDb::StringToCrc( "Blob" );
					break;
				}
			}

			EGDataAsset::SaveDataAsset( L"C:/projects/emergence/!credits.egasset" , CreditConv , EGReflection_GetEditor( *CreditConv , "DataAsset" ) );

			EGDeleteObject( CreditConv );
		}
	}
	*/
};

EG_CLASS_DECL( DeCreditsMenu )
