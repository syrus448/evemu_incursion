/*
	------------------------------------------------------------------------------------
	LICENSE:
	------------------------------------------------------------------------------------
	This file is part of EVEmu: EVE Online Server Emulator
	Copyright 2006 - 2008 The EVEmu Team
	For the latest information visit http://evemu.mmoforge.org
	------------------------------------------------------------------------------------
	This program is free software; you can redistribute it and/or modify it under
	the terms of the GNU Lesser General Public License as published by the Free Software
	Foundation; either version 2 of the License, or (at your option) any later
	version.

	This program is distributed in the hope that it will be useful, but WITHOUT
	ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
	FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License along with
	this program; if not, write to the Free Software Foundation, Inc., 59 Temple
	Place - Suite 330, Boston, MA 02111-1307, USA, or go to
	http://www.gnu.org/copyleft/lesser.txt.
	------------------------------------------------------------------------------------
	Author:		Zhur
*/
#ifndef EVE_ITEM_FACTORY_H
#define EVE_ITEM_FACTORY_H

#include <map>

#include "../common/packet_types.h"
#include "../common/gpoint.h"

#include "inventory/InventoryDB.h"
#include "inventory/ItemRef.h"

class ItemCategory;

class ItemGroup;

class ItemType;
class BlueprintType;
class CharacterType;
class ShipType;
class StationType;

class Inventory;

class ItemFactory {
	friend class InventoryItem;	//only for access to _DeleteItem
public:
	ItemFactory(DBcore &db, EntityList &el);
	~ItemFactory();
	
	EntityList &entity_list;	//we do not own this.
	InventoryDB &db() { return(m_db); }

	/*
	 * Category stuff
	 */
	const ItemCategory *GetCategory(EVEItemCategories category);

	/*
	 * Group stuff
	 */
	const ItemGroup *GetGroup(uint32 groupID);

	/*
	 * Type stuff
	 */
	const ItemType *GetType(uint32 typeID);

	const BlueprintType *GetBlueprintType(uint32 blueprintTypeID);

	/**
	 * Loads character type, caches it and returns it.
	 *
	 * @param[in] characterTypeID Character type to be returned.
	 * @return Pointer to character type data container; NULL if fails.
	 */
	const CharacterType *GetCharacterType(uint32 characterTypeID);
	/**
	 * Loads character type, caches it and returns it.
	 *
	 * @param[in] characterTypeID Character type to be returned.
	 * @return Pointer to character type data container; NULL if fails.
	 */
	const CharacterType *GetCharacterTypeByBloodline(uint32 bloodlineID);

	/**
	 * Loads ship type, caches it and returns it.
	 *
	 * @param[in] shipTypeID ID of ship type.
	 * @return Pointer to ship type data container; NULL if fails.
	 */
	const ShipType *GetShipType(uint32 shipTypeID);

	/**
	 * Loads station type, caches it and returns it.
	 *
	 * @param[in] stationTypeID ID of station type to load.
	 * @return Pointer to StationType object; NULL if fails.
	 */
	const StationType *GetStationType(uint32 stationTypeID);

	/*
	 * Item stuff
	 */
	InventoryItemRef GetItem(uint32 itemID);

	BlueprintRef GetBlueprint(uint32 blueprintID);

	/**
	 * Loads character.
	 *
	 * @param[in] character ID of character to load.
	 * @return Pointer to Character object; NULL if load failed.
	 */
	CharacterRef GetCharacter(uint32 characterID);

	/**
	 * Loads ship.
	 *
	 * @param[in] shipID ID of ship to load.
	 * @return Pointer to Ship object; NULL if failed.
	 */
	ShipRef GetShip(uint32 shipID);

	/**
	 * Loads celestial object.
	 *
	 * @param[in] celestialID ID of celestial object to load.
	 * @return Pointer to CelestialObject; NULL if fails.
	 */
	CelestialObjectRef GetCelestialObject(uint32 celestialID);

	/**
	 * Loads solar system.
	 *
	 * @param[in] solarSystemID ID of solar system to load.
	 * @return Pointer to solar system object; NULL if failed.
	 */
	SolarSystemRef GetSolarSystem(uint32 solarSystemID);

	/**
	 * Loads station.
	 *
	 * @param[in] stationID ID of station to load.
	 * @return Pointer to Station object; NULL if fails.
	 */
	StationRef GetStation(uint32 stationID);

	/**
	 * Loads skill.
	 *
	 * @param[in] skillID ID of skill to load.
	 * @return Pointer to Skill object; NULL if fails.
	 */
	SkillRef GetSkill(uint32 skillID);

	/**
	 * Loads owner.
	 *
	 * @param[in] ownerID ID of owner to load.
	 * @return Ref to Owner object.
	 */
	OwnerRef GetOwner(uint32 ownerID);

	//spawn a new item with the specified information, creating it in the DB as well.
	InventoryItemRef SpawnItem(ItemData &data);
	BlueprintRef SpawnBlueprint(ItemData &data, BlueprintData &bpData);
	/**
	 * Spawns new character, caches it and returns it.
	 *
	 * @param[in] data Item data (for entity table).
	 * @param[in] charData Character data.
	 * @param[in] appData Character's appearance.
	 * @param[in] corpData Character's corporation-membership data.
	 * @return Pointer to new Character object; NULL if spawn failed.
	 */
	CharacterRef SpawnCharacter(ItemData &data, CharacterData &charData, CharacterAppearance &appData, CorpMemberInfo &corpData);
	/**
	 * Spawns new ship.
	 *
	 * @param[in] data Item data for ship.
	 * @return Pointer to Ship object; NULL if failed.
	 */
	ShipRef SpawnShip(ItemData &data);
	/**
	 * Spawns new skill.
	 *
	 * @param[in] data Item data for skill.
	 * @return Pointer to new Skill object; NULL if fails.
	 */
	SkillRef SpawnSkill(ItemData &data);
	/**
	 * Spawns new owner.
	 *
	 * @param[in] data Item data for owner.
	 * @return Ref to new Owner object.
	 */
	OwnerRef SpawnOwner(ItemData &data);

	/*
	 * Inventory stuff
	 */
	Inventory *GetInventory(uint32 inventoryID, bool load=true);

protected:
	InventoryDB m_db;

	/*
	 * Member functions and variables:
	 */
	// Categories:
	std::map<EVEItemCategories, ItemCategory *> m_categories;

	// Groups:
	std::map<uint32, ItemGroup *> m_groups;

	// Types:
	template<class _Ty>
	const _Ty *_GetType(uint32 typeID);

	std::map<uint32, ItemType *> m_types;

	// Items:
	template<class _Ty>
	RefPtr<_Ty> _GetItem(uint32 itemID);

	void _DeleteItem(uint32 itemID);

	std::map<uint32, InventoryItemRef> m_items;
};


#endif

