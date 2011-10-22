#ifndef XPCRAFTLISTER_XPCRAFTRESOURCE_H
#define XPCRAFTLISTER_XPCRAFTRESOURCE_H

#include "Precomp.h"

class XPCraftResource {
public:
	XPCraftResource(
		std::map<std::string,std::string>* ArmorPieces_i,
		std::vector<std::string>* Genders_i,
		std::vector<std::string>* Races_i,
		std::vector<std::string>* ArmorVisualTypes_i,
		std::vector<std::string>* ItemClasses_i,
		std::string Model_i
	);

	bool IsVariation();
	bool IsArmorPart();
	bool IsModelPart();

	unsigned int GetVariation();
	std::string GetArmorVisualType();
	std::string ToIndex();
	std::string ToAVTIndex();
protected:
	std::string Model;
	std::string ModelType;
	unsigned int variation;
	std::string* Race;
	std::string* Gender;
	std::string* ArmorVisualType;
	std::string* ItemClass;
	std::string ItemClassSub;

	bool LoadFromModelName( std::string Model_i );
	
	std::map<std::string,std::string>* ArmorPieces;	// Remap, such as from LSHOULDER -> ACLtShoulder.
	std::vector<std::string>* Genders;				// 2DA::gender::GENDER.
	std::vector<std::string>* Races;				// 2DA::appearance::NWN2_Model_Helm.
	std::vector<std::string>* ArmorVisualTypes;		// 2DA::armorvisualdata::Prefix.
	std::vector<std::string>* ItemClasses;			// 2DA::baseitem::ItemClass + extras.
};

XPCraftResource::XPCraftResource(
	std::map<std::string,std::string>* ArmorPieces_i,
	std::vector<std::string>* Genders_i,
	std::vector<std::string>* Races_i,
	std::vector<std::string>* ArmorVisualTypes_i,
	std::vector<std::string>* ItemClasses_i,
	std::string Model_i ) {
	// Null out.
	Race = NULL;
	Gender = NULL;
	ArmorVisualType = NULL;
	ItemClass = NULL;

	// Position hooks.
	variation = 0;
	ArmorPieces = ArmorPieces_i;
	Genders = Genders_i;
	Races = Races_i;
	ArmorVisualTypes = ArmorVisualTypes_i;
	ItemClasses = ItemClasses_i;

	// Parse the name.
	if ( !LoadFromModelName( Model_i ) ) throw std::exception( "Could not parse model." );
	
	// Backup the model.
	Model = Model_i;
}

bool XPCraftResource::LoadFromModelName( std::string str ) {
	// Determine the base type.
	stringToUpperCase( str );
	size_t found = str.find( "_" );
	if ( found == std::string::npos ) return false;
	Model = str;
	ModelType = str.substr( 0, found );

	// Handle variations.
	if ( IsVariation() ) {
		// Find the race.
		for ( std::vector<std::string>::iterator i = Races->begin(); i < Races->end(); i++ ) {
			if ( str.find( *i ) == std::string::npos ) continue;
			Race = &*i;
		}
		if ( !Race || Race->empty() ) throw std::exception( "Could not gather race from model." );

		// Find the gender.
		size_t PosGender = str.find( *Race ) + Race->length();
		for ( std::vector<std::string>::iterator i = Genders->begin(); i < Genders->end(); i++ ) {
			if ( str.substr( PosGender, i->length() ) != *i ) continue;
			Gender = &*i;
		}
		if ( !Gender || Gender->empty() ) throw std::exception( "Could not gather gender from model." );

		// Find armor visual type.
		size_t PosArmorVisualType = PosGender + Gender->length() + 1;
		for ( std::vector<std::string>::iterator i = ArmorVisualTypes->begin(); i < ArmorVisualTypes->end(); i++ ) {
			if ( str.substr( PosArmorVisualType, i->length() ) != *i ) continue;
			ArmorVisualType = &*i;
		}
		if ( !ArmorVisualType || ArmorVisualType->empty() ) throw std::exception( "Could not gather Armor Visual Type from model." );

		// Find item class.
		size_t PosItemClass = PosArmorVisualType + ArmorVisualType->length() + 1;
		for ( std::vector<std::string>::iterator i = ItemClasses->begin(); i < ItemClasses->end(); i++ ) {
			if ( str.substr( PosItemClass, i->length() ) != *i ) continue;
			ItemClass = &*i;
		}
		if ( !ItemClass || ItemClass->empty() ) throw std::exception( "Could not gather Item Class from model." );

		// Get the variation.
		size_t PosVariation = PosItemClass + ItemClass->length();
		variation = boost::lexical_cast<unsigned int>( str.substr( PosVariation, str.length() - PosVariation ) );
		if ( variation < 0 || variation > 256 ) throw std::exception( "Could not gather variation from model." );
		return true;
	}

	if ( IsArmorPart() ) {
		// Temp set it to use base format.
		str.replace( 0, 1, "P" );

		// Find the race.
		for ( std::vector<std::string>::iterator i = Races->begin(); i < Races->end(); i++ ) {
			if ( str.find( *i ) == std::string::npos ) continue;
			Race = &*i;
		}
		if ( !Race || Race->empty() ) throw std::exception( "Could not gather race from model." );

		// Find the gender.
		size_t PosGender = Race->length();
		for ( std::vector<std::string>::iterator i = Genders->begin(); i < Genders->end(); i++ ) {
			if ( str.substr( PosGender, i->length() ) != *i ) continue;
			Gender = &*i;
		}
		if ( !Gender || Gender->empty() ) throw std::exception( "Could not gather gender from model." );

		// Find the armor part.
		size_t SizeArmorVisualType = 0;
		size_t PosArmorVisualType = PosGender + Gender->length() + 1;
		for ( std::map<std::string,std::string>::iterator i = ArmorPieces->begin(); i != ArmorPieces->end(); i++ ) {
			if ( str.substr( PosArmorVisualType, i->first.length() ) != i->first ) continue;
			ArmorVisualType = &i->second;
			SizeArmorVisualType = i->first.length();
		}
		if ( !ArmorVisualType || ArmorVisualType->empty() ) throw std::exception( "Could not gather Armor Visual Type from model." );

		// Get the variation.
		size_t PosVariation = PosArmorVisualType + SizeArmorVisualType;
		variation = boost::lexical_cast<unsigned int>( str.substr( PosVariation, str.length() - PosVariation ) );
		if ( variation < 0 || variation > 256 ) throw std::exception( "Could not gather variation from model." );
		return true;
	}

	if ( IsModelPart() ) {
		// Find item class.
		for ( std::vector<std::string>::iterator i = ItemClasses->begin(); i < ItemClasses->end(); i++ ) {
			if ( str.substr( 0, i->length() ) != *i ) continue;
			ItemClass = &*i;
		}
		if ( !ItemClass || ItemClass->empty() ) throw std::exception( "Could not gather Item Class from model." );

		// Find item class suffix/subclass.
		size_t found = str.find( "_", ItemClass->length() );
		if ( found != std::string::npos ) {
			ItemClassSub = str.substr( found + 1, 1 );
			found = 2;
		} else {
			ItemClassSub = "A";
			found = 0;
		}

		// Get the variation.
		size_t PosVariation = ItemClass->length();
		variation = boost::lexical_cast<unsigned int>( str.substr( PosVariation, str.length() - ItemClass->length() - found ) );
		if ( variation < 0 || variation > 256 ) throw std::exception( "Could not gather variation from model." );
		return true;
	}

	// Unknown type.
	return false;
}

bool XPCraftResource::IsVariation() {
	return ( ModelType == "P" );
}
bool XPCraftResource::IsArmorPart() {
	return ( ModelType == "A" );
}
bool XPCraftResource::IsModelPart() {
	return ( ModelType == "W" );
}

unsigned int XPCraftResource::GetVariation() {
	return variation;
}

std::string XPCraftResource::GetArmorVisualType() {
	return *ArmorVisualType;
}

std::string XPCraftResource::ToIndex() {
	// Variation.
	if ( IsVariation() ) {
		return ( "XC_" + *Race + *Gender + "_" + *ArmorVisualType + "_" + *ItemClass );
	}

	// Armor Part.
	if ( IsArmorPart() ) {
		return ( "XC_A" + Race->substr( 1, Race->length() - 1 ) + *Gender + "_" + *ArmorVisualType );
	}

	// Armor Part.
	if ( IsModelPart() ) {
		return ( "XC_" + *ItemClass + "_" + ItemClassSub );
	}

	// Unhandled!
	throw std::exception( "Could not convert XPCR to index." );
	return "";
}

std::string XPCraftResource::ToAVTIndex() {
	if ( !IsVariation() ) throw std::exception( "Invalid call of ToAVTIndex()." );
	return ( "XC_AVT_" + *Race + *Gender + "_" + *ItemClass );
}

#endif