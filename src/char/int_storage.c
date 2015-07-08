// Copyright (c) Athena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#include "../common/mmo.h"
#include "../common/malloc.h"
#include "../common/showmsg.h"
#include "../common/socket.h"
#include "../common/strlib.h" // StringBuf
#include "../common/sql.h"
#include "char.h"
#include "inter.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define STORAGE_MEMINC	16

/// Save storage data to sql
int storage_tosql(int account_id, struct storage_data* p)
{
	char_memitemdata_to_sql(p->items, MAX_STORAGE, account_id, TABLE_STORAGE);
	return 0;
}

/// Load storage data to mem
int storage_fromsql(int account_id, struct storage_data* p)
{
	StringBuf buf;
	int i, j;

	memset(p, 0, sizeof(struct storage_data)); //clean up memory
	p->storage_amount = 0;

	// storage {`account_id`/`id`/`nameid`/`amount`/`equip`/`identify`/`refine`/`attribute`/`card0`/`card1`/`card2`/`card3`}
	StringBuf_Init(&buf);
	StringBuf_AppendStr(&buf, "SELECT `id`,`nameid`,`amount`,`equip`,`identify`,`refine`,`attribute`,`expire_time`,`bound`,`unique_id`");
	for( j = 0; j < MAX_SLOTS; ++j )
		StringBuf_Printf(&buf, ",`card%d`", j);
	StringBuf_Printf(&buf, " FROM `%s` WHERE `account_id`='%d' ORDER BY `nameid`", schema_config.storage_db, account_id);

	if( SQL_ERROR == Sql_Query(sql_handle, StringBuf_Value(&buf)) )
		Sql_ShowDebug(sql_handle);

	StringBuf_Destroy(&buf);

	for( i = 0; i < MAX_STORAGE && SQL_SUCCESS == Sql_NextRow(sql_handle); ++i )
	{
		struct item* item;
		char* data;
		item = &p->items[i];
		Sql_GetData(sql_handle, 0, &data, NULL); item->id = atoi(data);
		Sql_GetData(sql_handle, 1, &data, NULL); item->nameid = atoi(data);
		Sql_GetData(sql_handle, 2, &data, NULL); item->amount = atoi(data);
		Sql_GetData(sql_handle, 3, &data, NULL); item->equip = atoi(data);
		Sql_GetData(sql_handle, 4, &data, NULL); item->identify = atoi(data);
		Sql_GetData(sql_handle, 5, &data, NULL); item->refine = atoi(data);
		Sql_GetData(sql_handle, 6, &data, NULL); item->attribute = atoi(data);
		Sql_GetData(sql_handle, 7, &data, NULL); item->expire_time = (unsigned int)atoi(data);
		Sql_GetData(sql_handle, 8, &data, NULL); item->bound = atoi(data);
		Sql_GetData(sql_handle, 9, &data, NULL); item->unique_id = strtoull(data, NULL, 10);
		for( j = 0; j < MAX_SLOTS; ++j ){
			Sql_GetData(sql_handle, 10+j, &data, NULL); item->card[j] = atoi(data);
		}
	}
	p->storage_amount = i;
	Sql_FreeResult(sql_handle);

	ShowInfo("storage load complete from DB - id: %d (total: %d)\n", account_id, p->storage_amount);
	return 1;
}

// DB -> storage data conversion
int ext_storage_fromsql(int account_id, struct extra_storage_data *p)
{
	StringBuf buf;
	struct item* item;
	char* data;
	int i;
	int j;

	memset(p, 0, sizeof(struct extra_storage_data)); //clean up memory
	p->storage_amount = 0;

	// storage {`account_id`/`id`/`nameid`/`amount`/`equip`/`identify`/`refine`/`attribute`/`card0`/`card1`/`card2`/`card3`}
	StringBuf_Init(&buf);
	StringBuf_AppendStr(&buf, "SELECT `id`,`nameid`,`amount`,`equip`,`identify`,`refine`,`attribute`,`expire_time`,`unique_id`,`bound`");
	for( j = 0; j < MAX_SLOTS; ++j )
		StringBuf_Printf(&buf, ",`card%d`", j);
	StringBuf_Printf(&buf, " FROM `%s` WHERE `account_id`='%d' ORDER BY `nameid`", schema_config.rentstorage_db, account_id);

	if( SQL_ERROR == Sql_Query(sql_handle, StringBuf_Value(&buf)) )
		Sql_ShowDebug(sql_handle);

	StringBuf_Destroy(&buf);

	for( i = 0; i < MAX_EXTRA_STORAGE && SQL_SUCCESS == Sql_NextRow(sql_handle); ++i )
	{
		item = &p->items[i];
		Sql_GetData(sql_handle, 0, &data, NULL); item->id = atoi(data);
		Sql_GetData(sql_handle, 1, &data, NULL); item->nameid = atoi(data);
		Sql_GetData(sql_handle, 2, &data, NULL); item->amount = atoi(data);
		Sql_GetData(sql_handle, 3, &data, NULL); item->equip = atoi(data);
		Sql_GetData(sql_handle, 4, &data, NULL); item->identify = atoi(data);
		Sql_GetData(sql_handle, 5, &data, NULL); item->refine = atoi(data);
		Sql_GetData(sql_handle, 6, &data, NULL); item->attribute = atoi(data);
		Sql_GetData(sql_handle, 7, &data, NULL); item->expire_time = (unsigned int)atoi(data);
		Sql_GetData(sql_handle, 8, &data, NULL); item->unique_id = strtoull(data, NULL, 10);
		Sql_GetData(sql_handle, 9, &data, NULL); item->bound = atoi(data);
		for( j = 0; j < MAX_SLOTS; ++j )
		{
			Sql_GetData(sql_handle, 10+j, &data, NULL); item->card[j] = atoi(data);
		}
	}
	p->storage_amount = i;
	Sql_FreeResult(sql_handle);

	ShowInfo("rentstorage load complete from DB - id: %d (total: %d)\n", account_id, p->storage_amount);
	return 1;
}

/// Save guild_storage data to sql
int guild_storage_tosql(int guild_id, struct guild_storage* p)
{
	char_memitemdata_to_sql(p->items, MAX_GUILD_STORAGE, guild_id, TABLE_GUILD_STORAGE);
	ShowInfo ("guild storage save to DB - guild: %d\n", guild_id);
	return 0;
}

/// Load guild_storage data to mem
int guild_storage_fromsql(int guild_id, struct guild_storage* p)
{
	StringBuf buf;
	int i, j;

	memset(p, 0, sizeof(struct guild_storage)); //clean up memory
	p->storage_amount = 0;
	p->guild_id = guild_id;

	// storage {`guild_id`/`id`/`nameid`/`amount`/`equip`/`identify`/`refine`/`attribute`/`card0`/`card1`/`card2`/`card3`}
	StringBuf_Init(&buf);
	StringBuf_AppendStr(&buf, "SELECT `id`,`nameid`,`amount`,`equip`,`identify`,`refine`,`attribute`,`bound`,`unique_id`");
	for( j = 0; j < MAX_SLOTS; ++j )
		StringBuf_Printf(&buf, ",`card%d`", j);
	StringBuf_Printf(&buf, " FROM `%s` WHERE `guild_id`='%d' ORDER BY `nameid`", schema_config.guild_storage_db, guild_id);

	if( SQL_ERROR == Sql_Query(sql_handle, StringBuf_Value(&buf)) )
		Sql_ShowDebug(sql_handle);

	StringBuf_Destroy(&buf);

	for( i = 0; i < MAX_GUILD_STORAGE && SQL_SUCCESS == Sql_NextRow(sql_handle); ++i )
	{
		struct item* item;
		char* data;
		item = &p->items[i];
		Sql_GetData(sql_handle, 0, &data, NULL); item->id = atoi(data);
		Sql_GetData(sql_handle, 1, &data, NULL); item->nameid = atoi(data);
		Sql_GetData(sql_handle, 2, &data, NULL); item->amount = atoi(data);
		Sql_GetData(sql_handle, 3, &data, NULL); item->equip = atoi(data);
		Sql_GetData(sql_handle, 4, &data, NULL); item->identify = atoi(data);
		Sql_GetData(sql_handle, 5, &data, NULL); item->refine = atoi(data);
		Sql_GetData(sql_handle, 6, &data, NULL); item->attribute = atoi(data);
		Sql_GetData(sql_handle, 7, &data, NULL); item->bound = atoi(data);
		Sql_GetData(sql_handle, 8, &data, NULL); item->unique_id = strtoull(data, NULL, 10);
		item->expire_time = 0;
		item->bound = 0;
		for( j = 0; j < MAX_SLOTS; ++j ){
			Sql_GetData(sql_handle, 9+j, &data, NULL); item->card[j] = atoi(data);
		}
	}
	p->storage_amount = i;
	Sql_FreeResult(sql_handle);

	ShowInfo("guild storage load complete from DB - id: %d (total: %d)\n", guild_id, p->storage_amount);
	return 0;
}

//---------------------------------------------------------
// storage data initialize
int inter_storage_sql_init(void)
{
	return 1;
}
// storage data finalize
void inter_storage_sql_final(void)
{
	return;
}

// Delete char storage
int inter_storage_delete(int account_id)
{
	if( SQL_ERROR == Sql_Query(sql_handle, "DELETE FROM `%s` WHERE `account_id`='%d'", schema_config.storage_db, account_id) )
		Sql_ShowDebug(sql_handle);
	return 0;
}
int inter_rentstorage_delete(int account_id) // [ZephStorage]
{
	if( SQL_ERROR == Sql_Query(sql_handle, "DELETE FROM `%s` WHERE `account_id`='%d'", schema_config.rentstorage_db, account_id) )
		Sql_ShowDebug(sql_handle);
	return 0;
}
int inter_guild_storage_delete(int guild_id)
{
	if( SQL_ERROR == Sql_Query(sql_handle, "DELETE FROM `%s` WHERE `guild_id`='%d'", schema_config.guild_storage_db, guild_id) )
		Sql_ShowDebug(sql_handle);
	return 0;
}

//---------------------------------------------------------
// packet from map server

int mapif_load_guild_storage(int fd,int account_id,int guild_id, char flag)
{
	if( SQL_ERROR == Sql_Query(sql_handle, "SELECT `guild_id` FROM `%s` WHERE `guild_id`='%d'", schema_config.guild_db, guild_id) )
		Sql_ShowDebug(sql_handle);
	else if( Sql_NumRows(sql_handle) > 0 )
	{// guild exists
		WFIFOHEAD(fd, sizeof(struct guild_storage)+13);
		WFIFOW(fd,0) = 0x3818;
		WFIFOW(fd,2) = sizeof(struct guild_storage)+13;
		WFIFOL(fd,4) = account_id;
		WFIFOL(fd,8) = guild_id;
		WFIFOB(fd,12) = flag; //1 open storage, 0 don't open
		guild_storage_fromsql(guild_id, (struct guild_storage*)WFIFOP(fd,13));
		WFIFOSET(fd, WFIFOW(fd,2));
		return 0;
	}
	// guild does not exist
	Sql_FreeResult(sql_handle);
	WFIFOHEAD(fd, 12);
	WFIFOW(fd,0) = 0x3818;
	WFIFOW(fd,2) = 12;
	WFIFOL(fd,4) = account_id;
	WFIFOL(fd,8) = 0;
	WFIFOSET(fd, 12);
	return 0;
}
int mapif_save_guild_storage_ack(int fd,int account_id,int guild_id,int fail)
{
	WFIFOHEAD(fd,11);
	WFIFOW(fd,0)=0x3819;
	WFIFOL(fd,2)=account_id;
	WFIFOL(fd,6)=guild_id;
	WFIFOB(fd,10)=fail;
	WFIFOSET(fd,11);
	return 0;
}

//---------------------------------------------------------
// packet from map server

int mapif_parse_LoadGuildStorage(int fd)
{
	RFIFOHEAD(fd);
	mapif_load_guild_storage(fd,RFIFOL(fd,2),RFIFOL(fd,6),1);
	return 0;
}

int mapif_parse_SaveGuildStorage(int fd)
{
	int guild_id;
	int len;

	RFIFOHEAD(fd);
	guild_id = RFIFOL(fd,8);
	len = RFIFOW(fd,2);

	if( sizeof(struct guild_storage) != len - 12 )
	{
		ShowError("inter storage: data size error %d != %d\n", sizeof(struct guild_storage), len - 12);
	}
	else
	{
		if( SQL_ERROR == Sql_Query(sql_handle, "SELECT `guild_id` FROM `%s` WHERE `guild_id`='%d'", schema_config.guild_db, guild_id) )
			Sql_ShowDebug(sql_handle);
		else if( Sql_NumRows(sql_handle) > 0 )
		{// guild exists
			Sql_FreeResult(sql_handle);
			guild_storage_tosql(guild_id, (struct guild_storage*)RFIFOP(fd,12));
			mapif_save_guild_storage_ack(fd, RFIFOL(fd,4), guild_id, 0);
			return 0;
		}
		Sql_FreeResult(sql_handle);
	}
	mapif_save_guild_storage_ack(fd, RFIFOL(fd,4), guild_id, 1);
	return 0;
}
#ifdef BOUND_ITEMS
int mapif_itembound_ack(int fd, int aid, int guild_id)
{
	WFIFOHEAD(fd,8);
	WFIFOW(fd,0) = 0x3856;
	WFIFOL(fd,2) = aid;
	WFIFOW(fd,6) = guild_id;
	WFIFOSET(fd,8);
	return 0;
}

//------------------------------------------------
//Guild bound items pull for offline characters [Akinari]
//------------------------------------------------
int mapif_parse_itembound_retrieve(int fd)
{
	StringBuf buf;
	SqlStmt* stmt;
	struct item item;
	int j, i = 0, s = 0, bound_qt = 0;
	bool found = false;
	struct item items[MAX_INVENTORY];
	unsigned int bound_item[MAX_INVENTORY] = { 0 };
	int char_id = RFIFOL(fd,2);
	int aid = RFIFOL(fd,6);
	int guild_id = RFIFOW(fd,10);

	StringBuf_Init(&buf);
	StringBuf_AppendStr(&buf, "SELECT `id`, `nameid`, `amount`, `equip`, `identify`, `refine`, `attribute`, `expire_time`, `bound`");
	for( j = 0; j < MAX_SLOTS; ++j )
		StringBuf_Printf(&buf, ", `card%d`", j);
	StringBuf_Printf(&buf, " FROM `%s` WHERE `char_id`='%d'",schema_config.inventory_db,char_id);

	stmt = SqlStmt_Malloc(sql_handle);
	if( SQL_ERROR == SqlStmt_PrepareStr(stmt, StringBuf_Value(&buf))
	||  SQL_ERROR == SqlStmt_Execute(stmt) )
	{
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		StringBuf_Destroy(&buf);
		mapif_itembound_ack(fd,aid,guild_id);
		return 1;
	}

	SqlStmt_BindColumn(stmt, 0, SQLDT_INT,       &item.id,          0, NULL, NULL);
	SqlStmt_BindColumn(stmt, 1, SQLDT_USHORT,    &item.nameid,      0, NULL, NULL);
	SqlStmt_BindColumn(stmt, 2, SQLDT_SHORT,     &item.amount,      0, NULL, NULL);
	SqlStmt_BindColumn(stmt, 3, SQLDT_USHORT,    &item.equip,       0, NULL, NULL);
	SqlStmt_BindColumn(stmt, 4, SQLDT_CHAR,      &item.identify,    0, NULL, NULL);
	SqlStmt_BindColumn(stmt, 5, SQLDT_CHAR,      &item.refine,      0, NULL, NULL);
	SqlStmt_BindColumn(stmt, 6, SQLDT_CHAR,      &item.attribute,   0, NULL, NULL);
	SqlStmt_BindColumn(stmt, 7, SQLDT_UINT,      &item.expire_time, 0, NULL, NULL);
	SqlStmt_BindColumn(stmt, 8, SQLDT_UINT,      &item.bound,       0, NULL, NULL);
	for( j = 0; j < MAX_SLOTS; ++j )
		SqlStmt_BindColumn(stmt, 9+j, SQLDT_SHORT, &item.card[j], 0, NULL, NULL);

	while( SQL_SUCCESS == SqlStmt_NextRow(stmt) ) {
		if(item.bound == BOUND_GUILD) {
			memcpy(&items[i],&item,sizeof(struct item));
			i++;
		}
	}
	Sql_FreeResult(sql_handle);

	if(!i) { //No items found - No need to continue
		StringBuf_Destroy(&buf);
		SqlStmt_Free(stmt);
		mapif_itembound_ack(fd,aid,guild_id);
		return 0;
	}

	//First we delete the character's items
	StringBuf_Clear(&buf);
	StringBuf_Printf(&buf, "DELETE FROM `%s` WHERE",schema_config.inventory_db);
	for(j=0; j<i; j++) {
		if( found )
			StringBuf_AppendStr(&buf, " OR");
		else
			found = true;
		StringBuf_Printf(&buf, " `id`=%d",items[j].id);

		if( items[j].bound && items[j].equip ) {
			//Only the items that are also stored in `char` `equip`
			if( (items[j].equip&EQP_HAND_R) ||
				(items[j].equip&EQP_HAND_L) ||
				(items[j].equip&EQP_HEAD_TOP) ||
				(items[j].equip&EQP_HEAD_MID) ||
				(items[j].equip&EQP_HEAD_LOW) ||
				(items[j].equip&EQP_GARMENT) ||
				(items[j].equip&EQP_COSTUME_HEAD_TOP) ||
				(items[j].equip&EQP_COSTUME_HEAD_MID) ||
				(items[j].equip&EQP_COSTUME_HEAD_LOW) ||
				(items[j].equip&EQP_COSTUME_GARMENT) )
			{
				bound_item[bound_qt] = items[j].equip;
				bound_qt++;
			}
		}
	}

	if( SQL_ERROR == SqlStmt_PrepareStr(stmt, StringBuf_Value(&buf))
	||  SQL_ERROR == SqlStmt_Execute(stmt) )
	{
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		StringBuf_Destroy(&buf);
		mapif_itembound_ack(fd,aid,guild_id);
		return 1;
	}

	if( bound_qt ) { //Removes any view id that was set by an item that was removed
/* Verifies equip bitmasks (see item.equip) and handles the sql statement */
#define CHECK_REMOVE(var,mask,token) do { \
	if( (var)&(mask) ) { \
		if( (var) != (mask) && s ) StringBuf_AppendStr(&buf, ","); \
		StringBuf_AppendStr(&buf, "`"#token"`='0'"); \
		(var) &= ~(mask); \
		s++; \
	} \
} while( 0 )

		StringBuf_Clear(&buf);
		StringBuf_Printf(&buf, "UPDATE `%s` SET ", schema_config.char_db);
		for( j = 0; j < bound_qt; j++ ) {
			//Equips can be at more than one slot at the same time
			CHECK_REMOVE(bound_item[j],EQP_HAND_R,weapon);
			CHECK_REMOVE(bound_item[j],EQP_HAND_L,shield);
			CHECK_REMOVE(bound_item[j],EQP_HEAD_TOP|EQP_COSTUME_HEAD_TOP,head_top);
			CHECK_REMOVE(bound_item[j],EQP_HEAD_MID|EQP_COSTUME_HEAD_MID,head_mid);
			CHECK_REMOVE(bound_item[j],EQP_HEAD_LOW|EQP_COSTUME_HEAD_LOW,head_bottom);
			CHECK_REMOVE(bound_item[j],EQP_GARMENT|EQP_COSTUME_GARMENT,robe);
		}
		StringBuf_Printf(&buf, " WHERE `char_id`='%d'", char_id);

		if( SQL_ERROR == SqlStmt_PrepareStr(stmt, StringBuf_Value(&buf)) ||
			SQL_ERROR == SqlStmt_Execute(stmt) )
		{
			SqlStmt_ShowDebug(stmt);
			SqlStmt_Free(stmt);
			StringBuf_Destroy(&buf);
			mapif_itembound_ack(fd,aid,guild_id);
			return 1;
		}
#undef CHECK_REMOVE
	}

	//Now let's update the guild storage with those deleted items
	//@TODO/FIXME:
	//This approach is basically the same as the one from memitemdata_to_sql, but
	//the latter compares current database values and this is not needed in this case
	//maybe sometime separate memitemdata_to_sql into different methods in order to use
	//call that function here as well [Panikon]
	found = false;
	StringBuf_Clear(&buf);
	StringBuf_Printf(&buf, "INSERT INTO `%s` (`guild_id`, `nameid`, `amount`, `equip`, `identify`, `refine`,"
		"`attribute`, `expire_time`, `bound`", schema_config.guild_storage_db);
	for( s = 0; s < MAX_SLOTS; ++s )
		StringBuf_Printf(&buf, ", `card%d`", s);
	StringBuf_AppendStr(&buf, ") VALUES ");

	for( j = 0; j < i; ++j ) {
		if( found )
			StringBuf_AppendStr(&buf, ",");
		else
			found = true;

		StringBuf_Printf(&buf, "('%d', '%hu', '%d', '%d', '%d', '%d', '%d', '%d', '%d'",
			guild_id, items[j].nameid, items[j].amount, items[j].equip, items[j].identify, items[j].refine,
			items[j].attribute, items[j].expire_time, items[j].bound);
		for( s = 0; s < MAX_SLOTS; ++s )
			StringBuf_Printf(&buf, ", '%hu'", items[j].card[s]);
		StringBuf_AppendStr(&buf, ")");
	}

	if( SQL_ERROR == SqlStmt_PrepareStr(stmt, StringBuf_Value(&buf))
	||  SQL_ERROR == SqlStmt_Execute(stmt) )
	{
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		StringBuf_Destroy(&buf);
		mapif_itembound_ack(fd,aid,guild_id);
		return 1;
	}

	StringBuf_Destroy(&buf);
	SqlStmt_Free(stmt);

	//Finally reload storage and tell map we're done
	mapif_load_guild_storage(fd,aid,guild_id,0);

	//If character is logged in char, disconnect, 
	/* @CHECKME [lighta]
	 * I suppose this was an attempt to avoid item duplication if the expelled user reconnect during the operation.
	 * well it's kinda ugly to expel someone like this, so I consider this as a hack.
	 * we better flag them so that they not allowed to reconnect during operation or flag it so we will flush those item on ram with the map ack.
	 * both way seem nicer for player.
	 */
	char_disconnect_player(aid);

	//Tell map-server the operation is over and it can unlock the storage
	mapif_itembound_ack(fd,aid,guild_id);
	return 0;
}
#endif

int inter_storage_parse_frommap(int fd)
{
	RFIFOHEAD(fd);
	switch(RFIFOW(fd,0)){
	case 0x3018: mapif_parse_LoadGuildStorage(fd); break;
	case 0x3019: mapif_parse_SaveGuildStorage(fd); break;
#ifdef BOUND_ITEMS
	case 0x3056: mapif_parse_itembound_retrieve(fd); break;
#endif
	default:
		return 0;
	}
	return 1;
}
