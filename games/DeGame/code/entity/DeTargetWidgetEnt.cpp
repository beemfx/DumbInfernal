// (c) 2018 Beem Media

#include "DeTargetWidgetEnt.h"
#include "EGVisualComponent.h"
#include "EGSettings2Types.h"

EG_CLASS_DECL( DeTargetWidgetEnt )

static EGSettingsBool DeTargetWidgetEnt_IsDebugVisible( "DeTargetWidgetEnt_IsDebugVisible" , eg_loc("DeTargetWidgetEnt_IsDebugVisibleName","DEBUG: Show Target Widget Entity") , false , EGS_F_DEBUG_EDITABLE|EGS_F_SYS_SAVED );

void DeTargetWidgetEnt::OnEnterWorld()
{
	Super::OnEnterWorld();

	HandleComponentVisibility();

	if( IsClient() )
	{
		DeTargetWidgetEnt_IsDebugVisible.OnChangedDelegate.AddUnique( this , &ThisClass::OnDebugVisibleChanged );
	}
}

void DeTargetWidgetEnt::OnLeaveWorld()
{
	if( IsClient() )
	{
		DeTargetWidgetEnt_IsDebugVisible.OnChangedDelegate.RemoveAll( this );
	}
	
	Super::OnLeaveWorld();
}

void DeTargetWidgetEnt::OnDebugVisibleChanged()
{
	HandleComponentVisibility();
}

void DeTargetWidgetEnt::HandleComponentVisibility()
{
	if( GetRole() != eg_ent_role::EditorPreview )
	{
		// In anything but the preview world hide any visual components.
		m_ComponentTree.IterateComponents( []( EGComponent* Comp ) -> void
		{
			EGVisualComponent* AsVisualComponent = EGCast<EGVisualComponent>( Comp );
			if( AsVisualComponent )
			{
				AsVisualComponent->SetHidden( !DeTargetWidgetEnt_IsDebugVisible.GetValue() );
			}
		} );
	}
}
