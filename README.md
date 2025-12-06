# DUMB Infernal

Updated: December 6, 2025

(c) 2025 Beem Media

Welcome to the release of the source code for DUMB Infernal. This repository includes the game code. The Emergence Engine is included as a submodule so it is recommended to use the --recursive with git clone so that everything needed is synced.

## Building

We have done our best to create as smooth as possible an experience to build the source code. You sould be able to build the source with a default installation of Visual Studio 2022 Community Edition that includes the "Desktop Development with C++" workload. A connection to the internet may be required for Visual Studio and MSBuild to acquire required NuGet packages.

Game assets are not included in this release, so if you want to run the game we recommend getting the game content from Steam (https://store.steampowered.com/app/1208370/DUMB_Infernal/).

To build the game open EG.sln with Visual Studio 2022. Set the Solution Configuration to "Debug" or "Release", set the Platform to "x64", set the default project to "ExGame". In the dub settings specify the debug working directory as the directory where the game content is located.

For example:
<pre>
C:\Program Files (x86)\Steam\steamapps\common\DUMB Infernal
</pre>

## About the Licensing

### Source Code

Beem Media is releasing the source code found in this repository under the license indicated by the LICENSE document found in the root directory of the repository. This applies to source code created by Beem Media. Additional source code may be subject to its own copyright and license, such instances will generally be noted as such in the source code itself or in adjacent documents.
