#include "plugin.h"

bool bGameSaInit = false;

CGameSA				*pGameInterface = NULL;
CPed				*pPedSelf = NULL;

struct pool			*pool_actor = nullptr;
struct pool			*pool_vehicle = nullptr;

bool Dark::GAME::Initialise()
{
	static bool init = false;

	if (init == false)
	{
		pool_actor = *(struct pool **)ACTOR_POOL_POINTER;
		if (pool_actor == nullptr || pool_actor->start == nullptr || pool_actor->size <= 0)
			return false;

		pool_vehicle = *(struct pool **)VEHICLE_POOL_POINTER;
		if (pool_vehicle == nullptr || pool_vehicle->start == nullptr || pool_vehicle->size <= 0)
			return false;

		struct actor_info	*actor_info;
		actor_info = getActorInfo(ACTOR_SELF, ACTOR_ALIVE);

		if (actor_info == nullptr) return false;

		pGameInterface = new CGameSA();
		pPedSelf = pGameInterface->GetPools()->AddPed((DWORD *)actor_info);

		init = true;
	}

	return true;
}

CGameSA *Dark::GAME::getGameInterface()
{
	if (pGameInterface == NULL) return NULL;

	return pGameInterface;
}

CPed *Dark::GAME::getPlayerPed()
{
	if (getGameInterface() == NULL || pPedSelf == NULL) return NULL;

	return pPedSelf;
}

int isBadPtr_GTA_pVehicle(vehicle_info *p_VehicleInfo)
{
	if (p_VehicleInfo == NULL)
		return 1;
	if (!
		(
		(DWORD)p_VehicleInfo >= (DWORD)pool_vehicle->start
			&& (DWORD)p_VehicleInfo <= ((DWORD)pool_vehicle->start + (pool_vehicle->size * sizeof(vehicle_info)))
			)
		)
		return 1;
	return (p_VehicleInfo->base.matrix == NULL) || (p_VehicleInfo->base.nType != ENTITY_TYPE_VEHICLE);
}

int isBadPtr_GTA_pPed(actor_info *pActorInfo)
{
	if (pActorInfo == NULL)
		return 1;
	if (!
		(
		(DWORD)pActorInfo >= (DWORD)pool_actor->start
			&& (DWORD)pActorInfo <= ((DWORD)pool_actor->start + (pool_actor->size * sizeof(actor_info)))
			)
		)
		return 1;
	return (pActorInfo->base.matrix == NULL) || (pActorInfo->base.nType != ENTITY_TYPE_PED);
}

struct actor_info *Dark::GAME::getActorInfo(int id, int flags)
{
	struct actor_info	*info;

	if (pool_actor == NULL) return NULL;
	if (id != ACTOR_SELF && (id < 0 && id >= pool_actor->size)) return NULL;

	if (id == ACTOR_SELF)
	{
		info = (struct actor_info *)(UINT_PTR) * (uint32_t *)ACTOR_POINTER_SELF;
		if (isBadPtr_GTA_pPed(info)) return NULL;
	}
	else
	{
		info = &((struct actor_info *)pool_actor->start)[id];
		if (isBadPtr_GTA_pPed(info)) return NULL;
	}

	if (vect3_near_zero(&info->base.matrix[4 * 3])) return NULL;
	if ((flags & ACTOR_ALIVE) && ACTOR_IS_DEAD(info)) return NULL;

	if (flags & ACTOR_NOT_SAME_VEHICLE)
	{
		struct actor_info	*self = getActorInfo(ACTOR_SELF, 0);

		if (self != NULL
			&&	 info->state == ACTOR_STATE_DRIVING
			&&	 self->state == ACTOR_STATE_DRIVING
			&&	 info->vehicle == self->vehicle) return NULL;
	}

	return info;
}

int vehicle_contains_actor(struct vehicle_info *vehicle, struct actor_info *actor)
{
	struct actor_info	*passengers[VEHICLE_MAX_PASSENGERS];
	struct actor_info	*temp;

	if (actor == NULL) return NULL;
	memcpy(passengers, vehicle->passengers, sizeof(uint32_t) * VEHICLE_MAX_PASSENGERS);

	int i;
	for (i = 0; i < VEHICLE_MAX_PASSENGERS; i++)
	{
		temp = passengers[i];
		if (temp == actor) return 1;
	}

	return NULL;
}

struct vehicle_info *Dark::GAME::getVehicleInfo(int id, int flags)
{
	struct vehicle_info *info;

	if (pool_vehicle == NULL) return NULL;
	if (id != VEHICLE_SELF && (id < 0 && id >= pool_vehicle->size)) return NULL;

	if (id == VEHICLE_SELF)
	{
		info = (struct vehicle_info *)(UINT_PTR) * (uint32_t *)VEHICLE_POINTER_SELF;
		if (isBadPtr_GTA_pVehicle(info)) return NULL;

		struct actor_info	*actor = getActorInfo(ACTOR_SELF, ACTOR_ALIVE);
		if (!vehicle_contains_actor(info, actor)) return NULL;
	}
	else
	{
		info = &((struct vehicle_info *)pool_vehicle->start)[id];
		if (isBadPtr_GTA_pVehicle(info)) return NULL;
	}

	if ((flags & VEHICLE_ALIVE) && (info->hitpoints < 250.0f && info->burn_timer == 0.0f)) return NULL;
	return info;
}
