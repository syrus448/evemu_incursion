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

#include "EvemuPCH.h"

ItemFactory::ItemFactory(DBcore &db, EntityList &el)
: entity_list(el),
  m_db(&db)
{
}

ItemFactory::~ItemFactory() {
	// items
	{
		std::map<uint32, InventoryItem *>::const_iterator cur, end;
		cur = m_items.begin();
		end = m_items.end();
		for(; cur != end; cur++) {
			// save item
			cur->second->Save(false);	// do not recurse
			cur->second->DecRef();
		}
	}
	// types
	{
		std::map<uint32, Type *>::const_iterator cur, end;
		cur = m_types.begin();
		end = m_types.end();
		for(; cur != end; cur++)
			delete cur->second;
	}
	// groups
	{
		std::map<uint32, Group *>::const_iterator cur, end;
		cur = m_groups.begin();
		end = m_groups.end();
		for(; cur != end; cur++)
			delete cur->second;
	}
	// categories
	{
		std::map<EVEItemCategories, Category *>::const_iterator cur, end;
		cur = m_categories.begin();
		end = m_categories.end();
		for(; cur != end; cur++)
			delete cur->second;
	}
}

const Category *ItemFactory::GetCategory(EVEItemCategories category) {
	std::map<EVEItemCategories, Category *>::iterator res = m_categories.find(category);
	if(res == m_categories.end()) {
		Category *cat = Category::Load(*this, category);
		if(cat == NULL)
			return NULL;

		// insert it into our cache
		res = m_categories.insert(
			std::make_pair(category, cat)
		).first;
	}
	return(res->second);
}

const Group *ItemFactory::GetGroup(uint32 groupID) {
	std::map<uint32, Group *>::iterator res = m_groups.find(groupID);
	if(res == m_groups.end()) {
		Group *group = Group::Load(*this, groupID);
		if(group == NULL)
			return NULL;

		// insert it into cache
		res = m_groups.insert(
			std::make_pair(groupID, group)
		).first;
	}
	return(res->second);
}

const Type *ItemFactory::GetType(uint32 typeID) {
	std::map<uint32, Type *>::iterator res = m_types.find(typeID);
	if(res == m_types.end()) {
		Type *type = Type::Load(*this, typeID);
		if(type == NULL)
			return NULL;

		// insert into cache
		res = m_types.insert(
			std::make_pair(typeID, type)
		).first;
	}
	return(res->second);
}

const BlueprintType *ItemFactory::GetBlueprintType(uint32 blueprintTypeID) {
	std::map<uint32, Type *>::iterator res = m_types.find(blueprintTypeID);
	if(res == m_types.end()) {
		BlueprintType *bt = BlueprintType::Load(*this, blueprintTypeID);
		if(bt == NULL)
			return NULL;

		res = m_types.insert(
			std::make_pair(blueprintTypeID, bt)
		).first;
	}
	return(static_cast<const BlueprintType *>(res->second));
}

const CharacterType *ItemFactory::GetCharacterType(uint32 characterTypeID) {
	std::map<uint32, Type *>::iterator res = m_types.find(characterTypeID);
	if(res == m_types.end()) {
		CharacterType *ct = CharacterType::Load(*this, characterTypeID);
		if(ct == NULL)
			return NULL;

		res = m_types.insert(
			std::make_pair(characterTypeID, ct)
		).first;
	}
	return(static_cast<const CharacterType *>(res->second));
}

const CharacterType *ItemFactory::GetCharacterTypeByBloodline(uint32 bloodlineID) {
	// Unfortunately, we have it indexed by typeID, so we must get it ...
	uint32 characterTypeID;
	if(!db().GetCharacterTypeByBloodline(bloodlineID, characterTypeID))
		return NULL;
	return GetCharacterType(characterTypeID);
}

const ShipType *ItemFactory::GetShipType(uint32 shipTypeID) {
	std::map<uint32, Type *>::iterator res = m_types.find(shipTypeID);
	if(res == m_types.end()) {
		ShipType *st = ShipType::Load(*this, shipTypeID);
		if(st == NULL)
			return NULL;

		res = m_types.insert(
			std::make_pair(shipTypeID, st)
		).first;
	}
	return(static_cast<const ShipType *>(res->second));
}

InventoryItem *ItemFactory::GetItem(uint32 itemID, bool recurse) {
	std::map<uint32, InventoryItem *>::iterator res = m_items.find(itemID);
	if(res == m_items.end()) {
		// load the item
		InventoryItem *i = InventoryItem::Load(*this, itemID, recurse);
		if(i == NULL)
			return NULL;

		//we keep the original ref.
		res = m_items.insert(
			std::make_pair(itemID, i)
		).first;
	} else if(recurse) {
		// ensure its recursively loaded
		if(!res->second->LoadContents(true))
			return NULL;
	}
	//we return new ref to the user.
	return(res->second->IncRef());
}

Blueprint *ItemFactory::GetBlueprint(uint32 blueprintID, bool recurse) {
	std::map<uint32, InventoryItem *>::iterator res = m_items.find(blueprintID);
	if(res == m_items.end()) {
		Blueprint *bi = Blueprint::Load(*this, blueprintID, recurse);
		if(bi == NULL)
			return NULL;

		res = m_items.insert(
			std::make_pair(blueprintID, bi)
		).first;
	} else if(recurse) {
		if(!res->second->LoadContents(true))
			return NULL;
	}
	return(static_cast<Blueprint *>(res->second->IncRef()));
}

Character *ItemFactory::GetCharacter(uint32 characterID, bool recurse) {
	std::map<uint32, InventoryItem *>::iterator res = m_items.find(characterID);
	if(res == m_items.end()) {
		Character *c = Character::Load(*this, characterID, recurse);
		if(c == NULL)
			return NULL;

		res = m_items.insert(
			std::pair<uint32, InventoryItem *>(characterID, c)
		).first;
	} else if(recurse) {
		if(!res->second->LoadContents(true))
			return NULL;
	}
	return(static_cast<Character *>(res->second->IncRef()));
}

InventoryItem *ItemFactory::SpawnItem(ItemData &data) {
	InventoryItem *i = InventoryItem::Spawn(*this, data);
	if(i == NULL)
		return NULL;

	// spawn successfull; store the ref
	m_items[i->itemID()] = i;
	// return additional ref
	return i->IncRef();
}

Blueprint *ItemFactory::SpawnBlueprint(ItemData &data, BlueprintData &bpData) {
	Blueprint *bi = Blueprint::Spawn(*this, data, bpData);
	if(bi == NULL)
		return NULL;

	m_items[bi->itemID()] = bi;
	return bi->IncRef();
}

Character *ItemFactory::SpawnCharacter(ItemData &data, CharacterData &charData, CharacterAppearance &appData, CorpMemberInfo &corpData) {
	Character *c = Character::Spawn(*this, data, charData, appData, corpData);
	if(c == NULL)
		return NULL;

	m_items[c->itemID()] = c;
	return c->IncRef();
}

InventoryItem *ItemFactory::_GetIfContentsLoaded(uint32 itemID) {
	std::map<uint32, InventoryItem *>::const_iterator res = m_items.find(itemID);
	if(res != m_items.end())
		if(res->second->ContentsLoaded())
			return res->second->IncRef();
	return NULL;
}
	
void ItemFactory::_DeleteItem(uint32 itemID) {
	std::map<uint32, InventoryItem *>::iterator res = m_items.find(itemID);
	if(res == m_items.end()) {
		codelog(SERVICE__ERROR, "Item ID %d not found when requesting deletion!", itemID);
		return;
	}

	res->second->DecRef();
	m_items.erase(res);
}




















