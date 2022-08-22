bool nig = false;

#include "stdafx.h"
#include "aimbot.hpp"
#include "funcs.hpp"
#include "driver.h"
#include "communication.hpp"
#include "../yesrender.h"
#include "utils.h"
#include "driver_utils.h"
#include "C:\Users\Momo\Desktop\r\Loader\External\Process.hpp"




#define XINPUT_GAMEPAD_TRIGGER_THRESHOLD    30
Controller::XboxController* Player1 = new Controller::XboxController(1);

DWORD64 base_addr;

typedef struct _FNlEntity {
	uint64_t Actor;
	int ID;
	uint64_t mesh;
	uint64_t root;
	uint64_t PlayerState;
	uint64_t LocalPlayerState;
	uint32_t LocalTeamID;
	uint32_t TeamID69;
	Vector3 pos;
	std::string name;
	bool Spotted;
}FNlEntity;

uint64_t offset_uworld;
uint64_t Uworld,
LocalPlayers,
PlayerController,
LocalPawn,
//LocalPlayerState,
LocalRootcomp,
Persistentlevel,
CameraManager;

float closestDistance = FLT_MAX;
DWORD_PTR closestPawn = NULL;
uint64_t closestMesh = NULL;

uint32_t localplayerID;
Vector3 LocalRelativeLocation; struct FBoxSphereBounds
{
	struct Vector3                                     Origin;                                                   // 0x0000(0x0018) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	struct Vector3                                     BoxExtent;                                                // 0x0018(0x0018) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	double                                             SphereRadius;                                             // 0x0030(0x0008) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
};


class FTextData {
public:
	char pad_0x0000[0x30];  //0x0000
	wchar_t* Name;          //0x0028 
	__int32 Length;         //0x0030 

};

struct FText {
	FTextData* Data;
	char UnknownData[0x10];

	wchar_t* Get() const {
		if (Data) {
			return Data->Name;
		}

		return nullptr;
	}
};


Vector3 m_CameraLocation;

bool isVisible(uint64_t mesh)
{
	if (!settings::vischeck)
		return true;

	if (!mesh)
		return false;
	float fLastSubmitTime = Read<float>(mesh + 0x330);
	float fLastRenderTimeOnScreen = Read<float>(mesh + 0x334);

	const float fVisionTick = 0.04f;
	bool bVisible = fLastRenderTimeOnScreen + fVisionTick >= fLastSubmitTime;
	return bVisible;
}
Vector3 vNeckOut;


char* wchar_to_char(const wchar_t* pwchar)
{
	int currentCharIndex = 0;
	char currentChar = pwchar[currentCharIndex];

	while (currentChar != '\0')
	{
		currentCharIndex++;
		currentChar = pwchar[currentCharIndex];
	}

	const int charCount = currentCharIndex + 1;

	char* filePathC = (char*)malloc(sizeof(char) * charCount);

	for (int i = 0; i < charCount; i++)
	{
		char character = pwchar[i];

		*filePathC = character;

		filePathC += sizeof(char);

	}
	filePathC += '\0';

	filePathC -= (sizeof(char) * charCount);

	return filePathC;
}

std::vector<FNlEntity> entityList;
std::vector<FNlEntity> itemList;

bool IsInScreen(Vector3 pos, int over = 30) {
	if (pos.x > -over && pos.x < Width + over && pos.y > -over && pos.y < Height + over) {
		return true;
	}
	else {
		return false;
	}
}


std::string readwtf(uintptr_t Address, void* Buffer, SIZE_T Size)
{

	ReadRaw(Address, Buffer, Size);

	char name[255] = { 0 };
	memcpy(&name, Buffer, Size);

	return std::string(name);
}

D3DMATRIX Dude_Matrix(Vector3 rot, Vector3 origin)
{
	float radPitch = (rot.x * float(M_PI) / 180.f);
	float radYaw = (rot.y * float(M_PI) / 180.f);
	float radRoll = (rot.z * float(M_PI) / 180.f);

	float SP = sinf(radPitch);
	float CP = cosf(radPitch);
	float SY = sinf(radYaw);
	float CY = cosf(radYaw);
	float SR = sinf(radRoll);
	float CR = cosf(radRoll);

	D3DMATRIX matrix;
	matrix.m[0][0] = CP * CY;
	matrix.m[0][1] = CP * SY;
	matrix.m[0][2] = SP;
	matrix.m[0][3] = 0.f;

	matrix.m[1][0] = SR * SP * CY - CR * SY;
	matrix.m[1][1] = SR * SP * SY + CR * CY;
	matrix.m[1][2] = -SR * CP;
	matrix.m[1][3] = 0.f;

	matrix.m[2][0] = -(CR * SP * CY + SR * SY);
	matrix.m[2][1] = CY * SR - CR * SP * SY;
	matrix.m[2][2] = CR * CP;
	matrix.m[2][3] = 0.f;

	matrix.m[3][0] = origin.x;
	matrix.m[3][1] = origin.y;
	matrix.m[3][2] = origin.z;
	matrix.m[3][3] = 1.f;

	return matrix;
}

FTransform Boneindex(DWORD_PTR mesh, int index) {
	DWORD_PTR bonesarray = Read<DWORD_PTR>(mesh + 0x590);
	if (bonesarray == NULL) {
		bonesarray = Read<DWORD_PTR>(mesh + 0x590 + 0x10);
	}
	return Read<FTransform>(bonesarray + (index * 0x60));
}



std::uintptr_t find_signature(const char* sig, const char* mask)
{
	auto buffer = std::make_unique<std::array<std::uint8_t, 0x100000>>();
	auto data = buffer.get()->data();

	for (std::uintptr_t i = 0u; i < (2u << 25u); ++i)
	{
		ReadRaw(sdk::module_base + i * 0x100000, data, 0x100000);

		if (!data)
			return 0;

		for (std::uintptr_t j = 0; j < 0x100000u; ++j)
		{
			if ([](std::uint8_t const* data, std::uint8_t const* sig, char const* mask)
				{
					for (; *mask; ++mask, ++data, ++sig)
					{
						if (*mask == 'x' && *data != *sig) return false;
					}
					return (*mask) == 0;
				}(data + j, (std::uint8_t*)sig, mask))
			{
				std::uintptr_t result = sdk::module_base + i * 0x100000 + j;
				std::uint32_t rel = 0;

				ReadRaw(result + 3, &rel, sizeof(std::uint32_t));

				if (!rel)
					return 0;

				return result + rel + 7;
			}
		}
	}

	return 0;
}

static std::string ReadGetNameFromFName(int key)
{
	uint32_t ChunkOffset = (uint32_t)((int)(key) >> 16);
	uint16_t NameOffset = (uint16_t)key;

	uint64_t NamePoolChunk = Read<uint64_t>(base_addr + 0xCC282C0 + (8 * ChunkOffset) + 16) + (unsigned int)(4 * NameOffset); //((ChunkOffset + 2) * 8) ERROR_NAME_SIZE_EXCEEDED
	uint16_t nameEntry = Read<uint16_t>(NamePoolChunk);

	int nameLength = nameEntry >> 6;
	char buff[1024];
	if ((uint32_t)nameLength)
	{
		for (int x = 0; x < nameLength; ++x)
		{
			buff[x] = Read<char>(NamePoolChunk + 4 + x);
		}
		char* v2 = buff; // rdi
		unsigned __int16* v3; // rbx
		signed int v4 = nameLength; // ebx
		uint64_t result; // ax
		signed int v6; // edx
		signed int v7; // ecx
		signed int v8; // eax
		unsigned int v9; // ecx

		result = 30;
		v6 = 0;
		v7 = 16;

		if (v4 > 0)
		{
			do
			{
				v7 = (unsigned int)(v6 | result);
				++v2;
				++v6;
			//	v8 = (byte)~(byte)v7;
				result = (unsigned int)(2 * v7);
				*(v2 - 1) ^= v8;
			} while (v6 < v4);
		} buff[nameLength] = '\0'; return std::string(buff);
	}
	else
	{
		return "";
	}
}
std::string GetNameFromFName(int key)
{

	uint32_t ChunkOffset = (uint32_t)((int)(key) >> 16);
	uint16_t NameOffset = (uint16_t)key;

	uint64_t NamePoolChunk = Read<uint64_t>(base_addr + 0xCC282C0 + (8 * ChunkOffset) + 16) + (unsigned int)(4 * NameOffset); //((ChunkOffset + 2) * 8) ERROR_NAME_SIZE_EXCEEDED
	if (Read<uint16_t>(NamePoolChunk) < 64)
	{
		auto a1 = Read<DWORD>(NamePoolChunk + 4);
		return ReadGetNameFromFName(a1);
	}
	else
	{
		return ReadGetNameFromFName(key);
	}
}

Vector3 GetBoneWithRotation(DWORD_PTR mesh, int id) {
	FTransform bone = Boneindex(mesh, id);
	FTransform ComponentToWorld = Read<FTransform>(mesh + 0x240);
	D3DMATRIX Matrix;
	Matrix = MatrixMultiplication(bone.ToMatrixWithScale(), ComponentToWorld.ToMatrixWithScale());
	return Vector3(Matrix._41, Matrix._42, Matrix._43);
}

struct CamewaDescwipsion
{
	float FieldOfView;
	Vector3 Rotation;
	Vector3 Location;
};



CamewaDescwipsion UndetectedCamera(__int64 a1)
{
	CamewaDescwipsion VirtualCamera;
	__int64 v1;
	__int64 v6;
	__int64 v7;
	__int64 v8;

	v1 = Read<__int64>((DWORD_PTR)LocalPlayers + 0xC8);
	__int64 v9 = Read<__int64>(v1 + 8);


		VirtualCamera.FieldOfView = 80.f / (float)((float)Read<double>(v9 + 0x690) / 1.19f);



	VirtualCamera.Rotation.x = Read<double>(v9 + 0x7E0);
	VirtualCamera.Rotation.y = Read<double>(a1 + 0x148);

	v6 = Read<__int64>((DWORD_PTR)LocalPlayers + 0x70);
	v7 = Read<__int64>(v6 + 0x98);
	v8 = Read<__int64>(v7 + 0xF8);

	VirtualCamera.Location = Read<Vector3>(v8 + 0x20);
	return VirtualCamera;
}



Vector3 ProjectWorldToScreen(Vector3 WorldLocation)
{
	CamewaDescwipsion vCamera = UndetectedCamera(LocalRootcomp);
	vCamera.Rotation.x = (asin(vCamera.Rotation.x)) * (180.0 / M_PI);

	//printf("Camera.x = %f", vCamera.Rotation.x);
//	printf("Camera.y = %f", vCamera.Rotation.y);

	D3DMATRIX tempMatrix = Dude_Matrix(vCamera.Rotation, Vector3(0, 0, 0));

	Vector3 vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	Vector3 vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	Vector3 vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

	Vector3 vDelta = WorldLocation - vCamera.Location;
	Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

	if (vTransformed.z < 1.f)
		vTransformed.z = 1.f;

	return Vector3((Width / 2.0f) + vTransformed.x * (((Width / 2.0f) / tanf(vCamera.FieldOfView * (float)M_PI / 360.f))) / vTransformed.z, (Height / 2.0f) - vTransformed.y * (((Width / 2.0f) / tanf(vCamera.FieldOfView * (float)M_PI / 360.f))) / vTransformed.z, 0);
}

bool valid_vector3(Vector3 vec3)
{
	return !(vec3.x == 0 && vec3.y == 0 && vec3.z == 0);
}



bool shouldaimController = false;
void controllerSupport() {
	while (true) {



		if (settings::controllersupport) {
			if (Player1->CheckConnection()) {

				if (Player1->GetControllerState().Gamepad.bLeftTrigger)
				{
					shouldaimController = true;
				}
				else {
					shouldaimController = false;
				}

			}


			if (nig) ExitThread(-1);

			if (settings::performancemode) {
				std::this_thread::sleep_for(std::chrono::milliseconds(2));
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(4));
		}
		else {
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}//it wasnt laggy with this before let me just cvhange something for pc users bcz i will forget later



	}
}

int actual_count = 0;

void fn_cache() {
	//hide_thread(LI_FN(GetCurrentThread).get()());
	int distance;

	while (true) {

		std::vector<FNlEntity> tmpList;
		std::vector<FNlEntity> tmpList2;

	//	if (!offset_uworld) offset_uworld = find_signature(skCrypt("\x48\x89\x05\x00\x00\x00\x00\x48\x8B\x4B\x78"), skCrypt("xxx????xxxx")) - sdk::module_base;


		Uworld = Read<uint64_t>(base_addr + 0xcbf3538);
		//	std::cout << "\nUworld: " << Uworld;
		uint64_t GameInstance = Read<uint64_t>(Uworld + offsets::game_instance);
			//std::cout << "\nGameInstance: " << GameInstance;
		LocalPlayers = Read<uint64_t>(Read<uint64_t>(GameInstance + offsets::local_players));
		//	std::cout << "\nLocalPlayers: " << LocalPlayers;
		PlayerController = Read<uint64_t>(LocalPlayers + offsets::player_controller);
		//std::cout << "\nLocalPlayers: " << LocalPlayers;
		CameraManager = Read<uint64_t>(PlayerController + offsets::player_camera_manager);
		//	std::cout << "\nCameraManager: " << CameraManager;
		LocalPawn = Read<uint64_t>(PlayerController + offsets::acknowledged_pawn);
		//std::cout << "\nLocalPawn: " << LocalPawn;
		LocalRootcomp = Read<uint64_t>(LocalPawn + offsets::root_component);
		//	std::cout << "\nLocalRootcomp: " << LocalRootcomp;
		LocalRelativeLocation = Read<Vector3>(LocalRootcomp + offsets::relative_loc);

		DWORD itemCount = Read<DWORD>(Uworld + (offsets::levels + sizeof(PVOID)));

		//	auto t = LI_FN(GetTickCount).get()();
		for (int i = 0; i < itemCount; i++) {
			uintptr_t ItemLevels = Read<uintptr_t>(Uworld + offsets::levels);
			if (!ItemLevels) return;

			uintptr_t ItemLevel = Read<uintptr_t>(ItemLevels + (i * sizeof(uintptr_t)));
			if (!ItemLevel) return;

			for (int i = 0; i < Read<DWORD>(ItemLevel + (offsets::aactors + sizeof(PVOID))); ++i) {

				uintptr_t ItemsPawns = Read<uintptr_t>(ItemLevel + offsets::aactors);
				uintptr_t CurrentItemPawn = Read<uintptr_t>(ItemsPawns + (i * sizeof(uintptr_t)));
				int currentitemid = Read<int>(CurrentItemPawn + offsets::item_definition);
				auto NameCurrentActor = GetNameFromFName(currentitemid);
			//	std::cout << "\ncurrentitemid: " << currentitemid;
			//	std::cout << "\nGetNameFromFName: " << NameCurrentActor;
				uint64_t mesh = Read<uint64_t>(CurrentItemPawn + offsets::mesh);

				uint64_t LocalPlayerState = Read<uint64_t>(LocalPawn + offsets::player_state);
				uint64_t PlayerState = Read<uint64_t>(CurrentItemPawn + offsets::player_state);
				uint64_t rootcomponent = Read<uint64_t>(CurrentItemPawn + offsets::root_component);
				uint32_t LocalTeamId = Read<uint32_t>(LocalPlayerState + offsets::team_index);
				uint32_t TeamId = Read<uint32_t>(PlayerState + offsets::team_index);

				uintptr_t ItemRootComponent = Read<uintptr_t>(CurrentItemPawn + offsets::root_component);
				Vector3 ItemPosition = Read<Vector3>(ItemRootComponent + offsets::relative_loc);

				if (strstr(NameCurrentActor.c_str(), skCrypt("FortPickupAthena")) || strstr(NameCurrentActor.c_str(), skCrypt("Tiered_Chest")) || strstr(NameCurrentActor.c_str(), skCrypt("Tiered_Ammo")) || strstr(NameCurrentActor.c_str(), skCrypt("Vehicle")))
				{
					FNlEntity fnlEntity{ };
					fnlEntity.Actor = CurrentItemPawn;
					fnlEntity.name = NameCurrentActor;
					fnlEntity.pos = ItemPosition;
					fnlEntity.root = rootcomponent;
					tmpList2.push_back(fnlEntity);

				}

				if (strstr(NameCurrentActor.c_str(), skCrypt("PlayerPawn_Athena_C"))
					|| strstr(NameCurrentActor.c_str(), skCrypt("PlayerPawn_Athena_Phoebe_C"))
					|| strstr(NameCurrentActor.c_str(), skCrypt("BP_MangPlayerPawn"))) {


				    	FNlEntity fnlEntity{ };
				
						fnlEntity.Actor = CurrentItemPawn;
						fnlEntity.mesh = mesh;
						fnlEntity.root = rootcomponent;
						fnlEntity.PlayerState = PlayerState;
						fnlEntity.LocalPlayerState = LocalPlayerState;
						fnlEntity.LocalTeamID = LocalTeamId;
						fnlEntity.TeamID69 = TeamId;
						fnlEntity.name = NameCurrentActor;
						fnlEntity.ID = currentitemid;


						tmpList.push_back(fnlEntity);
					
				}

			}
		}

		entityList = tmpList;
		itemList = tmpList2;


		if (LocalPawn && PlayerController)
		{
			auto localcurwep = Read<uint64_t>(LocalPawn + offsets::CurrWeapon);
			auto definition = Read<uint64_t>(localcurwep + offsets::weapondata);
			auto wtf = Read<uint64_t>(definition + offsets::item_definition);
			//std::cout << "niigger";
			auto names = GetNameFromFName(wtf);

			if (!definition) continue;

			if (names.find(skCrypt("SMG").decrypt()) != std::string::npos or names.find(skCrypt("Pistol").decrypt()) != std::string::npos) { 
				settings::aimspeed = settings::smg_smooth;
					//std::cout << "holding smg: " << names.c_str() << std::endl;
			}

			else if (names.find(skCrypt("Shotgun").decrypt()) != std::string::npos)
			{
				settings::aimspeed = settings::shotty_smooth;
					//std::cout << "holding shotgun: " << names.c_str() << std::endl;
			}
			else if (names.find(skCrypt("Assault").decrypt()) != std::string::npos or names.find(skCrypt("Thermal").decrypt()) != std::string::npos or names.find(skCrypt("Athena_R_Ore").decrypt()) != std::string::npos)
			{
				settings::aimspeed = settings::ar_smooth;
				//	std::cout << "holding ar: " << names.c_str() << std::endl;
			}
			else
			{
				settings::aimspeed = settings::other_smooth;
			}


		}

		//	t = LI_FN(GetTickCount).get()() - t;
		LI_FN(Sleep).get()(800);
	}

}


void actor_loop(forceinline::dx_renderer* renderer) {
	//hide_thread(LI_FN(GetCurrentThread).get()());
	float distance;


	auto entityListCopy = entityList;
	auto itemListCopy = itemList;
	//std::cout << "twest";
	for (auto entity : entityListCopy)
	{
		//std::cout << "twest2";
		if (LocalPawn == entity.Actor)continue;


		//uint64_t rootcomponent = driver.read<uint64_t>(entity.Actor + offsets::root_component);
		Vector3 Relativelocation = Read<Vector3>(entity.root + offsets::relative_loc); //this invalid
		Vector3 Relative_Location = ProjectWorldToScreen(Relativelocation);

		//printf("RelativeLoc.y : %f\n", Relative_Location.y);
		uint64_t mesh = entity.mesh;

		uint32_t LocalTeamId = entity.LocalTeamID;
		uint32_t TeamId = entity.TeamID69;


	
			//backtrack : write<float>(entity.Actor + 0xd4, 0.001f);


			distance = LocalRelativeLocation.Distance(Relativelocation) / 100.f;
#pragma region bones123
			Vector3 vHeadBone = GetBoneWithRotation(mesh, 68);
			Vector3 vHeadBoneOut = ProjectWorldToScreen(vHeadBone);


			Vector3 Pelvis = GetBoneWithRotation(mesh, 2);
			Vector3 PelvisOut = ProjectWorldToScreen(Pelvis);
			Vector3 vNeck = GetBoneWithRotation(mesh, 67);
			Vector3 vNeckOut = ProjectWorldToScreen(vNeck);

			Vector3 vRightFoot = GetBoneWithRotation(mesh, 80);
			Vector3 vRightFootOut = ProjectWorldToScreen(vRightFoot);

#pragma endregion

		
			if (distance <= settings::MaxDistance && IsInScreen(vNeckOut))
			{

				unsigned long ESPColor;
				unsigned long SkelColor;

				if (isVisible(mesh)) {

					ESPColor = 0xffdbb757;
					SkelColor = 0xffff0000;


					if (settings::enemyindicator)
					{
						renderer->draw_text(skCrypt(L"Enemy Is Visible").decrypt(), Width / 2, 80, 0xffffffff, true, true);
					}
				}
				else {

					ESPColor = 0xffff0000; //0xffff0000

					SkelColor = 0xff2dd0d3;

				}

				Vector3 player_position = GetBoneWithRotation(mesh, 0);
				Vector3 player_screen = ProjectWorldToScreen(player_position);

				Vector3 BoxHead = GetBoneWithRotation(mesh, 98);
				Vector3 head_screen = ProjectWorldToScreen(Vector3(BoxHead.x, BoxHead.y + 0.3, BoxHead.z));
				Vector3 esp_head_screen = ProjectWorldToScreen(Vector3(BoxHead.x, BoxHead.y, BoxHead.z + 15));
				Vector3 normal_head = ProjectWorldToScreen(BoxHead);

				float BoxHeight = player_screen.y - esp_head_screen.y;
				float BoxWidth = BoxHeight / 1.6f;

			

				if (settings::corner_box && LocalPawn && PlayerController)
				{
				//	renderer->draw_text(skCrypt(L"test").decrypt(), Width / 2, 80, 0xffffffff, true, true);
					draw_cornered_box(renderer, player_screen.x - (BoxWidth / 2), normal_head.y, BoxWidth, BoxHeight, 0xffff8c00);
				}

				if (settings::box && LocalPawn && PlayerController)
				{
					renderer->draw_rect(player_screen.x - (BoxWidth / 2), normal_head.y, BoxWidth, BoxHeight, 0xffff8c00);
				}

				if (settings::threed_box && LocalPawn && PlayerController) {
					FBoxSphereBounds EntityBounds = Read<FBoxSphereBounds>(entity.mesh + 0x738); //USkinnedMeshComponent	CachedWorldSpaceBounds	0x738	FBoxSphereBounds

					auto v000 = EntityBounds.Origin - EntityBounds.BoxExtent;
					auto v111 = EntityBounds.Origin + EntityBounds.BoxExtent;

					// top box
					auto v010 = Vector3(v000.x, v111.y, v000.z);
					auto v110 = Vector3(v111.x, v111.y, v000.z);
					auto v011 = Vector3(v000.x, v111.y, v111.z);

					// bottom box
					auto v101 = Vector3(v111.x, v000.y, v111.z);
					auto v100 = Vector3(v111.x, v000.y, v000.z);
					auto v001 = Vector3(v000.x, v000.y, v111.z);

					auto w1 = ProjectWorldToScreen(Vector3(v000.x, v000.y, v000.z));
					auto w2 = ProjectWorldToScreen(Vector3(v111.x, v111.y, v111.z));

					auto s1 = ProjectWorldToScreen(v010);
					auto s2 = ProjectWorldToScreen(v110);
					auto s3 = ProjectWorldToScreen(v011);
					auto s4 = ProjectWorldToScreen(v101);
					auto s5 = ProjectWorldToScreen(v100);
					auto s6 = ProjectWorldToScreen(v001);

					if (s1.x || s1.y) {
						renderer->draw_line(w1.x, w1.y, s6.x, s6.y, 0xffff8c00);
						renderer->draw_line(w1.x, w1.y, s5.x, s5.y, 0xffff8c00);
						renderer->draw_line(s4.x, s4.y, s5.x, s5.y, 0xffff8c00);
						renderer->draw_line(s4.x, s4.y, s6.x, s6.y, 0xffff8c00);

						renderer->draw_line(s1.x, s1.y, s3.x, s3.y, 0xffff8c00);
						renderer->draw_line(s1.x, s1.y, s2.x, s2.y, 0xffff8c00);
						renderer->draw_line(w2.x, w2.y, s2.x, s2.y, 0xffff8c00);
						renderer->draw_line(w2.x, w2.y, s3.x, s3.y, 0xffff8c00);

						renderer->draw_line(s6.x, s6.y, s3.x, s3.y, 0xffff8c00);
						renderer->draw_line(w1.x, w1.y, s1.x, s1.y, 0xffff8c00);
						renderer->draw_line(s4.x, s4.y, w2.x, w2.y, 0xffff8c00);
						renderer->draw_line(s5.x, s5.y, s2.x, s2.y, 0xffff8c00);
					}
				}


				if (settings::lines && LocalPawn && PlayerController)
				{
					renderer->draw_line((Width / 2), Height, normal_head.x, normal_head.y, 0xffffffff);
				}
				if (settings::name && LocalPawn && PlayerController)
				{
					std::string null = skCrypt("").decrypt();
					std::string Text;



					char buf[256];
					sprintf(buf, skCrypt("%dm"), (int)distance);


					std::wstring ws123(&buf[0], &buf[256]);

					std::wstring dupa;
					custom::StringToWString(dupa, Text);

					//std::wstring dupa2;
					//custom::StringToWString(dupa2, itemName);

					//renderer->draw_text(dupa, vHeadBoneOut.x - 10.f, vHeadBoneOut.y - 12, 0xffffffff);
					renderer->draw_text(ws123, vHeadBoneOut.x - 10.f, vHeadBoneOut.y - 12, 0xffffffff);
				}

				if (settings::playerweaponesp && LocalPawn && PlayerController)
				{
					auto curWep = Read<uint64_t>(entity.Actor + offsets::CurrWeapon);
					auto itemDef = Read<uint64_t>(curWep + offsets::weapondata); // -> UFortWeaponItemDefinition
					auto itemName = Read<uintptr_t>(itemDef + offsets::display_name);//all corect, checked not sure about second ones dont remove tier shit

					if (!itemName) continue;
					if (!itemDef) continue;
					auto bIsReloadingWeapon = Read<bool>(curWep + offsets::is_reloading);
					uint32_t StringLength = Read<uint32_t>(itemName + 0x38);

					if (StringLength <= 32) {
						auto itemName2 = Read<uintptr_t>(itemName + 0x30);

						auto tier = Read<BYTE>(itemDef + offsets::tier);

						unsigned long color;

						switch (tier) {
						case 0:
						case 1:
							color = 0xffa8a8a8;
							break;
						case 2:
							color = 0xff078f04;
							break;
						case 3:
							color = 0xff005cb8;
							break;
						case 4:
							color = 0xff700787;
							break;
						case 5:
							color = 0xffffc800;
							break;
						case 6:
							color = 0xffff950b;
							break;
						}

						if (!itemDef) continue;


						wchar_t* OutString = new wchar_t[uint64_t(StringLength) + 1];
						//readwtf(itemName2, OutString, StringLength * sizeof(wchar_t));
						ReadRaw((ULONG64)Read<PVOID>(itemName + 0x30), OutString, StringLength * sizeof(wchar_t));



						//wchar_t buffer[64];
						//readwtf(itemName2, &buffer, sizeof(wchar_t*) * 64);


						if (bIsReloadingWeapon)
							renderer->draw_text(skCrypt(L"Reloading").decrypt(), vHeadBoneOut.x - 10.f, vRightFootOut.y + 12, 0xffffffff);

						if (!bIsReloadingWeapon)

							renderer->draw_text(OutString, vHeadBoneOut.x - 10.f, vRightFootOut.y + 12, color);
					}

				}





				if (settings::aimbot && LocalPawn && PlayerController) {

					auto dx = head_screen.x - (Width / 2);
					auto dy = head_screen.y - (Height / 2);
					auto dist = sqrtf(dx * dx + dy * dy);

					if (dist < settings::aimfov * 3 && dist < closestDistance) {
						closestDistance = dist;
						closestPawn = entity.Actor;

					}
				}


			}
		

	}

	if (settings::chestesp || settings::ammoboxesp || settings::vehicle || settings::lootesp) {
		for (auto entity : itemListCopy)
		{
			//uintptr_t ItemRootComponent = driver.read<uintptr_t>(entity.Actor + offsets::root_component);
			//Vector3 ItemPosition = driver.read<Vector3>(ItemRootComponent + offsets::relative_loc);
			Vector3 ChestPosition = ProjectWorldToScreen(entity.pos);
			if (!LocalPawn && !PlayerController) continue;

			if (IsInScreen(ChestPosition) && LocalPawn && PlayerController) {
				float ItemDist = LocalRelativeLocation.Distance(entity.pos) / 100.f;
				bool bAlreadySearched = Read<bool>(entity.Actor + offsets::balreadysearched);
				std::string null = skCrypt("").decrypt();

				if (settings::chestesp && strstr(entity.name.c_str(), skCrypt("Tiered_Chest")) && !(((bAlreadySearched >> 7) & 1))) {

					if (ItemDist < 250.0f) {
						std::string Text;

						Text = null + skCrypt("Chest").decrypt();

						std::wstring dupa;
						custom::StringToWString(dupa, Text);

						//ImGui::GetOverlayDrawList()->AddText(ImVec2(ChestPosition.x - TextSize.x / 2, ChestPosition.y - TextSize.y / 2), SettingsColor::ChestESP, Text.c_str());
						renderer->draw_text(dupa, ChestPosition.x, ChestPosition.y, 0xffff950b, true, true);
						renderer->draw_text(skCrypt(L" [").decrypt() + std::to_wstring((int)ItemDist) + skCrypt(L"m]").decrypt(), ChestPosition.x, ChestPosition.y + 14, 0xffff950b, true, true);
					}
				}

				if (settings::ammoboxesp && strstr(entity.name.c_str(), skCrypt("Tiered_Ammo")) && !(((bAlreadySearched >> 7) & 1))) {


					if (ItemDist < 250.0f) {
						std::string Text;

						Text = null + skCrypt("AmmoBox [").decrypt();

						std::wstring dupa;
						custom::StringToWString(dupa, Text);

						renderer->draw_text(dupa, ChestPosition.x, ChestPosition.y, 0xffffffff, true, true);
						renderer->draw_text(skCrypt(L" [").decrypt() + std::to_wstring((int)ItemDist) + skCrypt(L"m]").decrypt(), ChestPosition.x, ChestPosition.y + 14, 0xffffffff, true, true);
					}
				}
				if (settings::vehicle && strstr(entity.name.c_str(), skCrypt("Vehicle"))) {
					if (ItemDist < 250.0f) {
						std::string Text;


						Text = null + skCrypt("Vehicle").decrypt();

						std::wstring dupa;
						custom::StringToWString(dupa, Text);

						renderer->draw_text(dupa, ChestPosition.x, ChestPosition.y, 0xffffffff, true, true);
						renderer->draw_text(skCrypt(L" [").decrypt() + std::to_wstring((int)ItemDist) + skCrypt(L"m]").decrypt(), ChestPosition.x, ChestPosition.y + 14, 0xffffffff, true, true);
					}
				}
				if (settings::lootesp && strstr(entity.name.c_str(), skCrypt("FortPickupAthena"))) {
					BYTE tier;
					if (ItemDist < 100.0f) {


						auto definition = Read<uint64_t>(entity.Actor + offsets::primary_pick_up_item_entry + offsets::item_definition); //0x308
						auto wtf = Read<uint64_t>(definition + offsets::item_definition);

						auto names = GetNameFromFName(wtf);

						tier = Read<BYTE>(definition + offsets::tier);


						auto DisplayName = Read<uint64_t>(definition + offsets::display_name); // DisplayName (FText)
						auto WeaponLength = Read<uint32_t>(DisplayName + 0x38); // FText -> Length
						wchar_t* WeaponName = new wchar_t[uint64_t(WeaponLength) + 1];

						ReadRaw((ULONG64)Read<PVOID>(DisplayName + 0x30), WeaponName, WeaponLength * sizeof(wchar_t));

						std::string Text = wchar_to_char(WeaponName);



						/*
						if (strstr(names.c_str(), skCrypt("RedDotBurstAR"))) {
							Text = null + skCrypt("AUG").decrypt();
						}
						else if (strstr(names.c_str(), skCrypt("SMG_Recoil"))) {
							Text = null + skCrypt("Combat SMG").decrypt();
						}
						else if (strstr(names.c_str(), skCrypt("SMG_CoreSMG"))) {
							Text = null + skCrypt("Stinger SMG").decrypt();
						}
						else if (strstr(names.c_str(), skCrypt("Assault_CoreAR"))) {
							Text = null + skCrypt("Ranger Assault Rifle").decrypt();
						}
						else if (strstr(names.c_str(), skCrypt("Assault_RedDotAR"))) {
							Text = null + skCrypt("Ranger Assault Rifle").decrypt();
						}
						else if (strstr(names.c_str(), skCrypt("Pistol"))) {
							Text = null + skCrypt("Pistol").decrypt();
						}
						else if (strstr(names.c_str(), skCrypt("Thermal"))) {
							Text = null + skCrypt("Thermal Scooped AR").decrypt();
						}
						else if (strstr(names.c_str(), skCrypt("Shotgun_Heavy"))) {
							Text = null + skCrypt("Heavy Shotgun").decrypt();
						}
						else if (strstr(names.c_str(), skCrypt("Shotgun_AutoDrum"))) {
							Text = null + skCrypt("Drum Shotgun").decrypt();
						}
						else if (strstr(names.c_str(), skCrypt("Shotgun_CoreBurst"))) {
							Text = null + skCrypt("Shotgun").decrypt();
						}
						else if (strstr(names.c_str(), skCrypt("Sniper"))) {
							Text = null + skCrypt("Sniper").decrypt();
						}
						else if (strstr(names.c_str(), skCrypt("ShieldSmall"))) {
							Text = null + skCrypt("Small Shield").decrypt();
						}
						else if (strstr(names.c_str(), skCrypt("Shields"))) {
							Text = null + skCrypt("Big Shield").decrypt();
						}
						else if (strstr(names.c_str(), skCrypt("ChilBronco"))) {
							Text = null + skCrypt("Chug Splash").decrypt();
						}
						else if (strstr(names.c_str(), skCrypt("SpicySoda"))) {
							Text = null + skCrypt("Chilli Splash").decrypt();
						}
						else if (strstr(names.c_str(), skCrypt("Flopper"))) {
							Text = null + skCrypt("Flopper").decrypt();
						}
						else if (strstr(names.c_str(), skCrypt("MetalItemData"))) {
							Text = null + skCrypt("Metal").decrypt();
						}
						else if (strstr(names.c_str(), skCrypt("WoodItemData"))) {
							Text = null + skCrypt("Wood").decrypt();
						}
						else if (strstr(names.c_str(), skCrypt("StoneItemData"))) {
							Text = null + skCrypt("Stone").decrypt();
						}
						else if (strstr(names.c_str(), skCrypt("Medkit"))) {
							Text = null + skCrypt("Medkit").decrypt();
						}
						else if (strstr(names.c_str(), skCrypt("HealSpray"))) {
							Text = null + skCrypt("Heal Spray").decrypt();
						}
						else if (strstr(names.c_str(), skCrypt("Bandage"))) {
							Text = null + skCrypt("Bandage").decrypt();
						}
						else if (strstr(names.c_str(), skCrypt("Athena_R_Ore"))) {
							Text = null + skCrypt("Assault Rifle").decrypt();
						}
						else if (strstr(names.c_str(), skCrypt("Assault_High"))) {
							Text = null + skCrypt("Assault Rifle").decrypt();
						}
						else if (strstr(names.c_str(), skCrypt("Assault_AutoHigh"))) {
							Text = null + skCrypt("Assault Rifle").decrypt();
						}
						else if (strstr(names.c_str(), skCrypt("Shotgun_Swing"))) {
							Text = null + skCrypt("Lever Action Shotgun").decrypt();
						}
						else if (strstr(names.c_str(), skCrypt("Shotgun_SemiAuto"))) {
							Text = null + skCrypt("Tactical Shotgun").decrypt();
						}
						else if (strstr(names.c_str(), skCrypt("Shotgun_Standard"))) {
							Text = null + skCrypt("Pump Shotgun").decrypt();
						}
						else if (strstr(names.c_str(), skCrypt("Shotgun_SlugFire"))) {
							Text = null + skCrypt("Heavy Shotgun").decrypt();
						}
						else if (strstr(names.c_str(), skCrypt("Pistol_AutoHeavy"))) {
							Text = null + skCrypt("SMG").decrypt();
						}
						else if (strstr(names.c_str(), skCrypt("Pistol_SixShooter"))) {
							Text = null + skCrypt("Six Shooter").decrypt();
						}
						else if (strstr(names.c_str(), skCrypt("Pistol_HandCannon"))) {
							Text = null + skCrypt("Hand Cannon").decrypt();
						}
						else if (strstr(names.c_str(), skCrypt("Pistol_Tactical"))) {
							Text = null + skCrypt("Pistol").decrypt();
						}
						else if (strstr(names.c_str(), skCrypt("Pistol_Standard"))) {
						Text = null + skCrypt("Supressed SMG").decrypt();
						}
						else {
							continue;
						}
						//continue;
						*/
						//Text = null + names.c_str();


						//std::string wtf2 = Text + skCrypt(" [").decrypt() + custom::to_string((int)ItemDist) + skCrypt("m]").decrypt();

						std::wstring dupa;
						custom::StringToWString(dupa, Text);

						unsigned long color;

						switch (tier) {
						case 0:
						case 1:
							color = 0xffa8a8a8;
							break;
						case 2:
							color = 0xff078f04;
							break;
						case 3:
							color = 0xff005cb8;
							break;
						case 4:
							color = 0xff700787;
							break;
						case 5:
							color = 0xffffc800;
							break;
						case 6:
							color = 0xffff950b;
							break;
						}

						renderer->draw_text(dupa, ChestPosition.x, ChestPosition.y, color, true, true);
						renderer->draw_text(skCrypt(L"[").decrypt() + std::to_wstring((int)ItemDist) + skCrypt(L"m]").decrypt(), ChestPosition.x, ChestPosition.y + 14, color, true, true);


					}
				}
			}
		}
	}

	if (closestPawn != 0 && LocalPawn && PlayerController) {
		if (settings::aimbot && (GetAsyncKeyState_Spoofed(thekey) || shouldaimController))
		{

			uint64_t currentactormesh = Read<uint64_t>(closestPawn + offsets::mesh);
			if (!currentactormesh) return;
			auto rootHead = GetBoneWithRotation(currentactormesh, thehitbox);
			//		if (!valid_vector3(rootHead)) return;
			Vector3 rootHeadOut = ProjectWorldToScreen(rootHead);
			//		if (!valid_vector3(rootHeadOut)) return;


			auto is_dying = (Read<char>(closestPawn + offsets::is_dying) >> 2) & 1;
			auto is_knocked = (Read<char>(closestPawn + offsets::is_dbno) >> 4) & 1;

			if (rootHeadOut.x != 0 or rootHeadOut.y != 0)
			{

				if (!is_dying)
				{
					closestDistance = FLT_MAX;
					closestPawn = NULL;
				}

				if (!settings::aimdowned)
				{
					if (is_knocked)
					{
						return;
					}
				}

		


				if (!isVisible(currentactormesh))
				{
					return;
				}



				if (settings::aimline)
					DrawLine(renderer, Width / 2, Height / 2, rootHeadOut.x, rootHeadOut.y, 0xffff0000);

				aimbot(rootHeadOut.x, rootHeadOut.y);
			}

		}
		else
		{
			closestDistance = FLT_MAX;
			closestPawn = NULL;
		}

	}


}


WPARAM MainLoop(forceinline::dx_renderer* renderer)
{
	static RECT old_rc;
	RtlSecureZeroMemory(&Message, sizeof(MSG));
	int buenotab = 0;
	int weapontab = 0;
	FVector2D pos = FVector2D{ 10,10 };

	superduper::SuperDuperRenderer menu_system = superduper::SuperDuperRenderer(renderer);

	while (Message.message != WM_QUIT)
	{
		if (PeekMessageA_Spoofed(&Message, hijacked_hwnd, 0, 0, PM_REMOVE)) //MyWnd
		{
			TranslateMessage_Spoofed(&Message);
			DispatchMessageA_Spoofed(&Message);
		}


		renderer->begin_rendering();

		//renderer->draw_text(std::to_wstring(renderer->get_fps()), 2, 2, 0xFFFFFFFF, false);

		HWND currHWND = GetForegroundWindow_Spoofed();
		FVector2D cursorpos = Input::CursorPos();

		if (currHWND == sdk::game_wnd) {

			Input::Handle();


			if (show_menu) {
				if (menu_system.Window(skCrypt("Momo der Libanese"), pos, FVector2D(400, 400), show_menu)) {
					if (menu_system.ButtonTab(skCrypt("Aimbot"), FVector2D(120, 24), buenotab == 0)) {
						buenotab = 0;
					}
					menu_system.SameLine();
					if (menu_system.ButtonTab(skCrypt("Visuals"), FVector2D(120, 24), buenotab == 1)) {
						buenotab = 1;
					}
					menu_system.SameLine();
					if (menu_system.ButtonTab(skCrypt("Misc"), FVector2D(120, 24), buenotab == 2)) {
						buenotab = 2;
					}

					menu_system.Separator();

					if (buenotab == 0)
					{
						menu_system.Checkbox(skCrypt("Aimbot"), &settings::aimbot);
						menu_system.SameLine();
						menu_system.Hotkey(skCrypt("Aim Key"), FVector2D(120, 20), &thekey);
						menu_system.Checkbox(skCrypt("Crosshair"), &settings::crosshair);
						menu_system.Checkbox(skCrypt("Circle FOV"), &settings::fovcircle);
						menu_system.Checkbox(skCrypt("Aim at downed"), &settings::aimdowned);
						menu_system.SliderInt(skCrypt("Circle FOV"), &settings::aimfov, 5, 360);
						menu_system.Separator();
						if (menu_system.ButtonTab(skCrypt("AR"), FVector2D(85, 24), weapontab == 0)) {
							weapontab = 0;
						}
						menu_system.SameLine();
						if (menu_system.ButtonTab(skCrypt("Shotgun"), FVector2D(85, 24), weapontab == 1)) {
							weapontab = 1;
						}
						menu_system.SameLine();
						if (menu_system.ButtonTab(skCrypt("SMG"), FVector2D(85, 24), weapontab == 2)) {
							weapontab = 2;
						}
						menu_system.SameLine();
						if (menu_system.ButtonTab(skCrypt("Other"), FVector2D(85, 24), weapontab == 2)) {
							weapontab = 3;
						}

						switch (weapontab) {
						case 0:
							menu_system.SliderInt(skCrypt("AR Smooth"), &settings::ar_smooth, 1, 10);
							menu_system.Combobox(skCrypt("AR Aimbone"), FVector2D(150, 20), &ar_index, skCrypt("Head").decrypt(), skCrypt("Neck").decrypt(), skCrypt("Chest").decrypt(), skCrypt("Pelvis").decrypt(), skCrypt("Random").decrypt(), NULL);//sec //test if work
							break;
						case 1:
							menu_system.SliderInt(skCrypt("Shotgun Smooth"), &settings::shotty_smooth, 1, 10);
							menu_system.Combobox(skCrypt("Shotgun Aimbone"), FVector2D(150, 20), &shotgun_index, skCrypt("Head").decrypt(), skCrypt("Neck").decrypt(), skCrypt("Chest").decrypt(), skCrypt("Pelvis").decrypt(), skCrypt("Random").decrypt(), NULL);//sec //test if work
							break;
						case 2:
							menu_system.SliderInt(skCrypt("SMG Smooth"), &settings::smg_smooth, 1, 10);
							menu_system.Combobox(skCrypt("SMG Aimbone"), FVector2D(150, 20), &smg_index, skCrypt("Head").decrypt(), skCrypt("Neck").decrypt(), skCrypt("Chest").decrypt(), skCrypt("Pelvis").decrypt(), skCrypt("Random").decrypt(), NULL);//sec //test if work

							break;
						case 3:
							menu_system.SliderInt(skCrypt("Other Smooth"), &settings::other_smooth, 1, 10);
							menu_system.Combobox(skCrypt("Other Aimbone"), FVector2D(150, 20), &other_index, skCrypt("Head").decrypt(), skCrypt("Neck").decrypt(), skCrypt("Chest").decrypt(), skCrypt("Pelvis").decrypt(), skCrypt("Random").decrypt(), NULL);//sec //test if work

							break;
						}

					}

					if (buenotab == 1)
					{
						menu_system.Checkbox(skCrypt("2D Box ESP"), &settings::box);
						menu_system.Checkbox(skCrypt("Corner Box ESP"), &settings::corner_box);
						menu_system.Checkbox(skCrypt("3D Box ESP"), &settings::threed_box);

						menu_system.Checkbox(skCrypt("Player Distance"), &settings::name);
						menu_system.Checkbox(skCrypt("Player Weapon"), &settings::playerweaponesp);
						menu_system.Checkbox(skCrypt("Snaplines"), &settings::lines);
						menu_system.Checkbox(skCrypt("Target line"), &settings::aimline);
						menu_system.Checkbox(skCrypt("Chest ESP"), &settings::chestesp);
						menu_system.Checkbox(skCrypt("Ammobox ESP"), &settings::ammoboxesp);
						menu_system.Checkbox(skCrypt("Vehicle ESP"), &settings::vehicle);
						menu_system.Checkbox(skCrypt("Loot ESP"), &settings::lootesp);
					}
					if (buenotab == 2)
					{
						menu_system.Checkbox(skCrypt("Performance Mode"), &settings::performancemode);
						menu_system.Checkbox(skCrypt("Controller Mode"), &settings::controllersupport);
						menu_system.Checkbox(skCrypt("Player Indicator"), &settings::enemyindicator);
						menu_system.Checkbox(skCrypt("Visible Check"), &settings::vischeck);
					}

				}

				int geyx = cursorpos.X;
				int geyy = cursorpos.Y;
				renderer->draw_line((geyx)-settings::crosshair_size, (geyy), (geyx)+(settings::crosshair_size + 1), (geyy), 0xffffffff);
				renderer->draw_line((geyx), (geyy)-settings::crosshair_size, (geyx), (geyy)+(settings::crosshair_size + 1), 0xffffffff);
			}

			actor_loop(renderer);

		}





		if (settings::fovcircle) {
			renderer->draw_circle(D3DXVECTOR2(Width / 2, Height / 2), settings::aimfov * 3, 8, 2, 0xffffffff);
		}
		if (settings::crosshair && !show_menu) {
			renderer->draw_line((Width / 2) - settings::crosshair_size, (Height / 2), (Width / 2) + (settings::crosshair_size + 1), (Height / 2), 0xffffffff);
			renderer->draw_line((Width / 2), (Height / 2) - settings::crosshair_size, (Width / 2), (Height / 2) + (settings::crosshair_size + 1), 0xffffffff);
		}




		renderer->end_rendering();

	

		static bool toggle = true;
		if (currHWND == sdk::game_wnd) {
			if (toggle) {
				SetWindowPos_Spoofed(hijacked_hwnd, HWND_TOPMOST, 0, 0, Width, Height, SWP_NOSIZE | SWP_NOMOVE);
				SetWindowPos_Spoofed(hijacked_hwnd, HWND_NOTOPMOST, 0, 0, Width, Height, SWP_NOSIZE | SWP_NOMOVE);

				//SetWindowLongPtrA(hijacked_hwnd, GWL_STYLE, WS_VISIBLE);
				//SetWindowLongPtrA(hijacked_hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT);

				ShowWindow_Spoofed(hijacked_hwnd, SW_SHOW);

				toggle = false;

			}
		}
		else {
			ShowWindow_Spoofed(hijacked_hwnd, SW_HIDE);
			toggle = true;
		}

		if (settings::performancemode) {
			std::this_thread::sleep_for(std::chrono::milliseconds(4));
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		//	std::this_thread::sleep_for(std::chrono::milliseconds(1));
			//maybe we put this in another thread () wait nvm it only exececutes once

		
	}


	CleanuoD3D();

	return Message.wParam;
}

void selfdestruct()
{
	while (true)
	{
		if (GetAsyncKeyState_Spoofed(VK_F9) & 1)
		{		
			nig = true;
			hijacked_hwnd = 0;
			sdk::game_wnd = 0;
			ShowWindow_Spoofed(hijacked_hwnd, SW_HIDE);
	
			show_menu = false;
			settings::aimbot = false;
			settings::fovcircle = false;
			settings::crosshair = false;
			settings::ammoboxesp = false;
			settings::chestesp = false;
			settings::aimline = false;
			settings::threed_box = false;
			settings::corner_box = false;
			settings::box = false;
			break;

			
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	}
}




const UINT sGetProcessId(const wchar_t* process_name) {
	UINT pid = 0;

	DWORD dwThreadCountMax = 0;

	// Create toolhelp snapshot.
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 process;
	ZeroMemory(&process, sizeof(process));
	process.dwSize = sizeof(process);
	// Walkthrough all processes.
	if (Process32First(snapshot, &process))
	{
		do
		{
			if (wcsstr(process.szExeFile, process_name))
			{
				DWORD dwTmpThreadCount = GetProcessThreadNumByID(process.th32ProcessID);
				if (dwTmpThreadCount > dwThreadCountMax)
				{
					dwThreadCountMax = dwTmpThreadCount;
					pid = process.th32ProcessID;
					break;
				}
			}
		} while (Process32Next(snapshot, &process));
	}
	CloseHandle(snapshot);
	return pid;
}


void init()
{


	ProcessId = GetProcessID(skCrypt("FortniteClient-Win64-Shipping.exe"));

	while (!ProcessId) {
		ProcessId = GetProcessID(skCrypt("FortniteClient-Win64-Shipping.exe"));
	}

	std::cout << "Process ID: " << ProcessId << "\n";

	while (!process_base) {
		process_base = GetProcessBase();
	}

	std::cout << "Base Address: " << (void*)process_base << "\n";


	Width = GetSystemMetrics_Spoofed(SM_CXSCREEN);
	Height = GetSystemMetrics_Spoofed(SM_CYSCREEN);

	hijack::init();

	DirectXInit(hijacked_hwnd);
	forceinline::dx_renderer renderer = forceinline::dx_renderer(p_Device);

	HANDLE handle = CreateThread_Spoofed(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(fn_cache), nullptr, NULL, nullptr);
	CloseHandle_Spoofed(handle);

	MainLoop(&renderer);
}

bool rpm() {
	uintptr_t addr_Read = Read<uintptr_t>(process_base);
	std::cout << "Read: " << addr_Read << "\n";
	Sleep(1000);
	return true;
}
int main() {
	start_service();

	init();

	while (true)
	{
		rpm();
	}
	return 0;
}
