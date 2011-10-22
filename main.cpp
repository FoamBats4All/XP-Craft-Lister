#include "precomp.h"
#include "XPCraftResource.h"

int __cdecl main( int argc, char** argv ) {
	PrintfTextOut TextOut;
	try {
		// Read the ini file.
		boost::program_options::options_description ini_desc;
		ini_desc.add_options()
			( "settings.module", "Module to load data from." )
			( "settings.ignoresingles", "Comment out single-variation lists." )
			( "paths.nwn2-install", "Neverwinter Nights 2 installation path." )
			( "paths.nwn2-home", "Neverwinter Nights 2 home directory." )
			( "invisiblemodels.modelparts", "Export header for armor visual types." )
			( "invisiblemodels.variations", "Export header for armor visual types." )
			( "invisiblemodels.armorpieces", "Export header for armor visual types." );
		std::ifstream ini_file( "XPCraftLister.ini" );
		boost::program_options::variables_map ini;
		boost::program_options::store( boost::program_options::parse_config_file( ini_file, ini_desc, true ), ini );

		// Set up output.
		std::ofstream nss( "nwnx_craft_set_constants.NSS" );

		// Get the check module.
		if ( !ini.count( "settings.module" ) ) throw std::exception( "Module not set." );
		std::string ModuleName = ini["settings.module"].as<std::string>();

		// Get the NWN2 install location.
		if ( !ini.count( "paths.nwn2-install" ) ) throw std::exception( "NWN2 install location not set." );
		std::string PathNWN2Install = ini["paths.nwn2-install"].as<std::string>();
		if ( !boost::filesystem::exists( PathNWN2Install ) ) throw std::exception( "NWN2 install location does not exist." );

		// Get the NWN2 home location.
		if ( !ini.count( "paths.nwn2-home" ) ) throw std::exception( "NWN2 home location not set." );
		std::string PathNWN2Home = ini["paths.nwn2-home"].as<std::string>();
		if ( !boost::filesystem::exists( PathNWN2Home ) ) throw std::exception( "NWN2 home location does not exist." );

		// Load the module.
		ResourceManager resources( &TextOut );
		TextOut.WriteText( "Loading '%s' ...", ModuleName.c_str() );
		LoadModule( resources, ModuleName.c_str(), PathNWN2Home.c_str(), PathNWN2Install.c_str() );

		// Gather: Genders.
		TextOut.WriteText( "\nParsing 2da files..." );
		std::vector<std::string> Genders;
		size_t RowCount = resources.Get2DARowCount( "gender" );
		for ( size_t Row = 0; Row < RowCount; Row++ ) {
			std::string Name;
			if ( !resources.Get2DAString( "gender", "GENDER", Row, Name ) ) continue;
			if ( Name == "*" ) continue;
			stringToUpperCase( Name );
			Genders.push_back( Name );
		}

		// Gather: Races.
		std::vector<std::string> Races;
		RowCount = resources.Get2DARowCount( "appearance" );
		for ( size_t Row = 0; Row < RowCount; Row++ ) {
			std::string Name;
			if ( !resources.Get2DAString( "appearance", "NWN2_Model_Helm", Row, Name ) ) continue;
			if ( Name == "****" ) continue;
			stringToUpperCase( Name );
			Races.push_back( Name.substr( 0, Name.length() - 1 ) );	// Don't forget to trim the '?'!
		}
		Races.push_back( "P_HA" );	// Missing: Halfling.
		Races.push_back( "P_AA" );	// Missing: Aasimar.
		Races.push_back( "P_GY" );	// Missing: Genasi.

		// Gather: Armor Visual Types.
		std::vector<std::string> ArmorVisualTypes;
		std::map<std::string,size_t> ArmorVisualTypeIndexMap;
		RowCount = resources.Get2DARowCount( "armorvisualdata" );
		for ( size_t Row = 0; Row < RowCount; Row++ ) {
			std::string Name;
			if ( !resources.Get2DAString( "armorvisualdata", "Prefix", Row, Name ) ) continue;
			if ( Name == "****" ) continue;
			stringToUpperCase( Name );
			ArmorVisualTypes.push_back( Name );
			ArmorVisualTypeIndexMap[Name] = Row;
		}

		// Gather: Item Classes.
		std::vector<std::string> ItemClasses;
		RowCount = resources.Get2DARowCount( "baseitems" );
		for ( size_t Row = 0; Row < RowCount; Row++ ) {
			std::string Name;
			if ( !resources.Get2DAString( "baseitems", "ItemClass", Row, Name ) ) continue;
			if ( Name == "*" ) continue;
			stringToUpperCase( Name );
			ItemClasses.push_back( Name );
		}
		ItemClasses.push_back( "BELT" );
		ItemClasses.push_back( "BODY" );
		ItemClasses.push_back( "BOOTS" );
		ItemClasses.push_back( "CLOAK" );
		ItemClasses.push_back( "GLOVES" );
		ItemClasses.push_back( "HELM" );

		// Gather: Item Class Suffix
		std::vector<std::string> ItemClassSuffix;
		ItemClassSuffix.push_back( "A" );
		ItemClassSuffix.push_back( "B" );
		ItemClassSuffix.push_back( "C" );

		// Gather: Armor Pieces.
		std::map<std::string,std::string> ArmorPieces;
		ArmorPieces["LSHOULDER"] = "ACLtShoulder";
		ArmorPieces["RSHOULDER"] = "ACRtShoulder";
		ArmorPieces["LBRACER"] = "ACLtBracer";
		ArmorPieces["RBRACER"] = "ACRtBracer";
		ArmorPieces["LELBOW"] = "ACLtElbow";
		ArmorPieces["RELBOW"] = "ACRtElbow";
		ArmorPieces["LUPARM"] = "ACLtArm";
		ArmorPieces["RUPARM"] = "ACRtArm";
		ArmorPieces["LHIP"] = "ACLtHip";
		ArmorPieces["RHIP"] = "ACRtHip";
		ArmorPieces["FHIP"] = "ACFtHip";
		ArmorPieces["BHIP"] = "ACBkHip";
		ArmorPieces["LUPLEG"] = "ACLtLeg";
		ArmorPieces["RUPLEG"] = "ACRtLeg";
		ArmorPieces["LLOWLEG"] = "ACLtShin";
		ArmorPieces["RLOWLEG"] = "ACRtShin";
		ArmorPieces["LKNEE"] = "ACLtKnee";
		ArmorPieces["RKNEE"] = "ACRtKnee";
		ArmorPieces["LFOOT"] = "ACLtFoot";
		ArmorPieces["RFOOT"] = "ACRtFoot";
		ArmorPieces["LANKEL"] = "ACLtAnkle";
		ArmorPieces["LANKEL"] = "ACRtAnkle";


		// Declare model containers.
		std::vector<XPCraftResource*> ModelParts;
		std::vector<XPCraftResource*> Variations;
		std::vector<XPCraftResource*> ArmorParts;
		std::vector<std::pair<std::string,std::string>> DiscardedModels;

		// Parse model list.
		TextOut.WriteText( "\nParsing model list..." );
		for ( ResourceManager::FileId i = resources.GetEncapsulatedFileCount(); i != 0; i-- ) {
			// Get the resource.
			NWN::ResRef32 ResRef;
			NWN::ResType ResType;
			if ( !resources.GetEncapsulatedFileEntry( ( i - 1 ), ResRef, ResType ) ) continue;

			// Is it a model?
			if ( ResType != NWN::ResMDB ) continue;

			// Get the model name.
			std::string ResName = resources.StrFromResRef( ResRef );
			stringToUpperCase( ResName );

			// Test output.
			XPCraftResource* model = NULL;
			try {
				model = new XPCraftResource( &ArmorPieces, &Genders, &Races, &ArmorVisualTypes, &ItemClasses, ResName );
			} catch ( std::exception &e ) {
				if ( model ) delete model;
				DiscardedModels.push_back( std::pair<std::string,std::string>( ResName, e.what() ) );
				continue;
			}
			
			// Store data.
			if ( model->IsModelPart() ) ModelParts.push_back( model );
			else if ( model->IsVariation() ) Variations.push_back( model );
			else if ( model->IsArmorPart() ) ArmorParts.push_back( model );
			else delete model;
		}
		
		// Output header.
		TextOut.WriteText( "\nGenerating NSS file..." );
		nss << "  /*--------------------------------------------------------------------------*\\\n"
			<< "  |                        nwnx_craft_set_constants                            |\n"
			<< "  |                          XP Craft - Carter DC                              |\n"
			<< "  |----------------------------------------------------------------------------|\n"
			<< "  |                      Generated by XP Craft Lister                          |\n"
			<< "  |                      Version 1.0.0 - FoamBats4All                          |\n"
			<< "  |----------------------------------------------------------------------------|\n"
			<< "  | Lists are stored as local variables, rather than string constants so that  |\n"
			<< "  | module builders can easily change (or in this case generate) lists.        |\n"
			<< "  |                                                                            |\n"
			<< "  | Lists are stored in the format: '#0#1#2#3#4#5#6#7#8#9#10#'                 |\n"
			<< "  |                                                                            |\n"
			<< "  | Lists with one value are useless, but are listed for future changes.       |\n"
			<< "  \\*--------------------------------------------------------------------------*/\n"
			<< "\nvoid main() {\n"
			<< "\tobject oModule = GetModule();\n"
			<< "\tobject oSpawnWP = GetWaypointByTag( \"XC_HIDDEN_WP\" );\n"
			<< "\tobject oHiddenContainer = CreateObject( OBJECT_TYPE_PLACEABLE, \"plc_colbox\", GetLocation( oSpawnWP ) );\n"
			<< "\tSetLocalObject( oModule, \"XC_HIDDEN_CONTAINER\", oHiddenContainer );";
		
		// Gather Model parts.
		std::map<std::string,std::vector<unsigned long>> ModelPartList;
		nss << "\n\n"
			<< "  /*--------------------------------------------------------------------------*\\\n"
			<< "  |                               ModelParts                                   |\n"
			<< "  |----------------------------------------------------------------------------|\n"
			<< "  | Model parts are related to weapons and shields. Some weapons have up to    |\n"
			<< "  | three model parts (i.e., longsword).                                       |\n"
			<< "  |                                                                            |\n"
			<< "  | 0 is a valid model part, but it represents an invisible part.              |\n"
			<< "  |                                                                            |\n"
			<< "  | These lists use the ItemClass column of the baseitem.2da 2DA file in order |\n"
			<< "  | to respect the official naming conventions of the weapon 3d models.        |\n"
			<< "  |                                                                            |\n"
			<< "  | If you ever use custom weapons types, you would need to name them in       |\n"
			<< "  | relation to that 2da file.                                                 |\n"
			<< "  \\*--------------------------------------------------------------------------*/";
		for ( std::vector<XPCraftResource*>::iterator i = ModelParts.begin(); i < ModelParts.end(); i++ ) {
			if ( std::find( ModelPartList[(*i)->ToIndex()].begin(), ModelPartList[(*i)->ToIndex()].end(), (*i)->GetVariation() ) != ModelPartList[(*i)->ToIndex()].end() ) continue;
			ModelPartList[(*i)->ToIndex()].push_back( (*i)->GetVariation() );
		}
		for ( std::map<std::string,std::vector<unsigned long>>::iterator i = ModelPartList.begin(); i != ModelPartList.end(); i++ ) {
			if ( ini["invisiblemodels.modelparts"].as<std::string>() == "1" ) i->second.push_back( 0 );
			std::sort( i->second.begin(), i->second.end() );
			if ( ini["settings.ignoresingles"].as<std::string>() == "1" && i->second.size() > 1 ) nss << "\n//";
			else nss << "\n";
			nss << "\tSetLocalString( oModule, \"" << i->first << "\", \"#";
			for ( std::vector<unsigned long>::iterator v = i->second.begin(); v < i->second.end(); v++ ) nss << *v << "#";
			nss << "\" );";
		}

		// Gather variations.
		std::map<std::string,std::vector<unsigned long>> VariationList;
		nss << "\n\n"
			<< "  /*--------------------------------------------------------------------------*\\\n"
			<< "  |                              Variations                                    |\n"
			<< "  |----------------------------------------------------------------------------|\n"
			<< "  | Variations are related to armor/clothes and other equipments (boots,       |\n"
			<< "  | helms, gloves, cloaks..) appearances, and are enslaved to ArmorVisualType  |\n"
			<< "  | values (cloth, banded, leather, chain, naked, etc.).                       |\n"
			<< "  |                                                                            |\n"
			<< "  | These lists are created according to the 3D model filenames, which start   |\n"
			<< "  | at index 0. The toolset starts listing these values at index 1. If you     |\n"
			<< "  | are using the toolset to modify these values, keep this in mind.           |\n"
			<< "  |                                                                            |\n"
			<< "  | Since the craft system uses the 2DA values NWN2_Model_Body and             |\n"
			<< "  | nwn2_Model_Helm, you can add new lists for new playable races without      |\n"
			<< "  | having to modify the system scripts.                                       |\n"
			<< "  |                                                                            |\n"
			<< "  | Variations can be set unique to each race and sex. If you do not want or   |\n"
			<< "  | need to separate based on race or gender, edit the nwnx_craft_system.nss   |\n"
			<< "  | file with the following changes:                                           |\n"
			<< "  |     XP_CRAFT_USE_RACED_VARIATION = FALSE;                                  |\n"
			<< "  |     XP_CRAFT_USE_SEXED_VARIATION = FALSE;                                  |\n"
			<< "  | This will default all races and genders to use human male armor parts.     |\n"
			<< "  | Because most vault material is designed for females, it is suggested that  |\n"
			<< "  | you do not switch sexed variation off.                                     |\n"
			<< "  |                                                                            |\n"
			<< "  | CL_BOOTS, CL_BODY, and CL_GLOVES variation 99 is an invisible model.       |\n"
			<< "  \\*--------------------------------------------------------------------------*/";
		for ( std::vector<XPCraftResource*>::iterator i = Variations.begin(); i < Variations.end(); i++ ) {
			if ( std::find( VariationList[(*i)->ToIndex()].begin(), VariationList[(*i)->ToIndex()].end(), (*i)->GetVariation() ) != VariationList[(*i)->ToIndex()].end() ) continue;
			VariationList[(*i)->ToIndex()].push_back( (*i)->GetVariation() );
		}

		for ( std::map<std::string,std::vector<unsigned long>>::iterator i = VariationList.begin(); i != VariationList.end(); i++ ) {
			if ( ini["invisiblemodels.variations"].as<std::string>() == "1" ) i->second.push_back( 0 );
			std::sort( i->second.begin(), i->second.end() );
			if ( ini["settings.ignoresingles"].as<std::string>() == "1" && i->second.size() > 1 ) nss << "\n//";
			else nss << "\n";
			nss << "\tSetLocalString( oModule, \"" << i->first << "\", \"#";
			for ( std::vector<unsigned long>::iterator v = i->second.begin(); v < i->second.end(); v++ ) nss << *v << "#";
			nss << "\" );";
		}

		// Gather armor pieces.
		std::map<std::string,std::vector<unsigned long>> ArmorPartList;
		nss << "\n\n"
			<< "  /*--------------------------------------------------------------------------*\\\n"
			<< "  |                               ArmorParts                                   |\n"
			<< "  |----------------------------------------------------------------------------|\n"
			<< "  | ArmorParts are related to armor components, such as bracers, shoulders,    |\n"
			<< "  | elbows, shins, knees, et cetera.                                           |\n"
			<< "  |                                                                            |\n"
			<< "  | 0 is a valid armor part, and respresents no armor part display.            |\n"
			<< "  |                                                                            |\n"
			<< "  | Armor parts can be set unique to each race and sex. If you do not want or  |\n"
			<< "  | need to separate based on race or gender, edit the nwnx_craft_system.nss   |\n"
			<< "  | file with the following changes:                                           |\n"
			<< "  |     XP_CRAFT_USE_RACED_ARMOR_PART = FALSE;                                 |\n"
			<< "  |     XP_CRAFT_USE_SEXED_ARMOR_PART = FALSE;                                 |\n"
			<< "  | This will default all races and genders to use human male armor parts.     |\n"
			<< "  |                                                                            |\n"
			<< "  | Invalid armor parts will not be displayed.                                 |\n"
			<< "  |                                                                            |\n"
			<< "  | 3D file names and GFF values are not the same, and are remapped:           |\n"
			<< "  |     LSHOULDER->ACLtShoulder                                                |\n"
			<< "  |     RSHOULDER->ACRtShoulder                                                |\n"
			<< "  |     LBRACER->ACLtBracer                                                    |\n"
			<< "  |     RBRACER->ACRtBracer                                                    |\n"
			<< "  |     LELBOW->ACLtElbow                                                      |\n"
			<< "  |     RELBOW->ACRtElbow                                                      |\n"
			<< "  |     LUPARM->ACLtArm                                                        |\n"
			<< "  |     RUPARM->ACRtArm                                                        |\n"
			<< "  |     LHIP->ACLtHip                                                          |\n"
			<< "  |     RHIP->ACRtHip                                                          |\n"
			<< "  |     LUPLEG->ACLtLeg                                                        |\n"
			<< "  |     RUPLEG->ACRtLeg                                                        |\n"
			<< "  |     LLOWLEG->ACLtShin                                                      |\n"
			<< "  |     RLOWLEG->ACRtShin                                                      |\n"
			<< "  |     LKNEE->ACLtKnee                                                        |\n"
			<< "  |     RKNEE->ACRtKnee                                                        |\n"
			<< "  \\*--------------------------------------------------------------------------*/";
		for ( std::vector<XPCraftResource*>::iterator i = ArmorParts.begin(); i < ArmorParts.end(); i++ ) {
			if ( std::find( ArmorPartList[(*i)->ToIndex()].begin(), ArmorPartList[(*i)->ToIndex()].end(), (*i)->GetVariation() ) != ArmorPartList[(*i)->ToIndex()].end() ) continue;
			ArmorPartList[(*i)->ToIndex()].push_back( (*i)->GetVariation() );
		}
		for ( std::map<std::string,std::vector<unsigned long>>::iterator i = ArmorPartList.begin(); i != ArmorPartList.end(); i++ ) {
			if ( ini["invisiblemodels.armorpieces"].as<std::string>() == "1" ) i->second.push_back( 0 );
			std::sort( i->second.begin(), i->second.end() );
			if ( ini["settings.ignoresingles"].as<std::string>() == "1" && i->second.size() > 1 ) nss << "\n//";
			else nss << "\n";
			nss << "\tSetLocalString( oModule, \"" << i->first << "\", \"#";
			for ( std::vector<unsigned long>::iterator v = i->second.begin(); v < i->second.end(); v++ ) nss << *v << "#";
			nss << "\" );";
		}

		// Calculate armor visual types.
		std::map<std::string,std::vector<unsigned long>> ArmorVisualTypeList;
		nss << "\n\n"
			<< "  /*--------------------------------------------------------------------------*\\\n"
			<< "  |                           Armor Visual Types                               |\n"
			<< "  |----------------------------------------------------------------------------|\n"
			<< "  | ArmorVisualTypes are related to the material category of 3d models, (i.e.  |\n"
			<< "  | scale, cloth, banded, leather, studded leather, chain, plate, ACME, etc.). |\n"
			<< "  | It does not alter the item's AC, only its visual armor type.               |\n"
			<< "  |                                                                            |\n"
			<< "  | Since the craft system uses the 2DA values NWN2_Model_Body and             |\n"
			<< "  | nwn2_Model_Helm, you can add new lists for new playable races without      |\n"
			<< "  | having to modify the system scripts.                                       |\n"
			<< "  |                                                                            |\n"
			<< "  | armorvisualdata.2da declares the model prefix for all 3d models.           |\n"
			<< "  \\*--------------------------------------------------------------------------*/";
		for ( std::vector<XPCraftResource*>::iterator i = Variations.begin(); i < Variations.end(); i++ ) {
			if ( std::find( ArmorVisualTypeList[(*i)->ToAVTIndex()].begin(), ArmorVisualTypeList[(*i)->ToAVTIndex()].end(),  ArmorVisualTypeIndexMap[(*i)->GetArmorVisualType()] ) != ArmorVisualTypeList[(*i)->ToAVTIndex()].end() ) continue;
			ArmorVisualTypeList[(*i)->ToAVTIndex()].push_back( ArmorVisualTypeIndexMap[(*i)->GetArmorVisualType()] );
		}
		for ( std::map<std::string,std::vector<unsigned long>>::iterator i = ArmorVisualTypeList.begin(); i != ArmorVisualTypeList.end(); i++ ) {;
			std::sort( i->second.begin(), i->second.end() );
			nss << "\n\tSetLocalString( oModule, \"" << i->first << "\", \"#";
			for ( std::vector<unsigned long>::iterator v = i->second.begin(); v < i->second.end(); v++ ) nss << *v << "#";
			nss << "\" );";
		}

		// Statistics.
		TextOut.WriteText( "\n\nStatistics:" );
		TextOut.WriteText( "\nWeapon Models: %u", ModelParts.size() );
		TextOut.WriteText( "\nArmor Variations: %u", Variations.size() );
		TextOut.WriteText( "\nArmor Parts: %u", ArmorParts.size() );
		TextOut.WriteText( "\nIgnored Models: %u", DiscardedModels.size() );

		// Output footer.
		nss << "\n}";
	} catch ( std::exception &e ) {
		TextOut.WriteText( "\nERROR: '%s'", e.what() );
	}

	// Exit
	return 0;
}