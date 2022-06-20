#include "memory.h"
#include "vector.h"
#include <thread>
#include "offsets.hpp"

struct Color
{
	std::uint8_t r{}, g{}, b{};
};

constexpr Vector3 CalculateAngle(
	const Vector3& localPosition,
	const Vector3& enemyPosition,
	const Vector3& viewAngles) noexcept
{
	return ((enemyPosition - localPosition).ToAngle() - viewAngles);
}
int main()
{
	std::cout << "Welcome to SensWare!\nAimbot key = SHIFT\nVisuals are visible only\nVersion:1.0";
	const auto memory = Memory{ "csgo.exe" };
	const auto client = memory.GetModuleAddress("client.dll");
	const auto engine = memory.GetModuleAddress("engine.dll");

	constexpr const auto teamColor = Color{ 0,0,255 };
	constexpr const auto enemyColor = Color{ 255,0,0 };
	

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		const auto& localPlayer = memory.Read<std::uintptr_t>(client + offset::dwLocalPlayer);
		const auto& localTeam = memory.Read<std::int32_t>(localPlayer + offset::m_iTeamNum);
		const auto localEyePosition = memory.Read<Vector3>(localPlayer + offset::m_vecOrigin) +
			memory.Read<Vector3>(localPlayer + offset::m_vecViewOffset);
		const auto& clientState = memory.Read<std::uintptr_t>(engine + offset::dwClientState);
		const auto& viewAngles = memory.Read<Vector3>(clientState + offset::dwClientState_ViewAngles);
		const auto& aimPunch = memory.Read<Vector3>(localPlayer + offset::m_viewPunchAngle) * 2;

		//SETTINGS
		float brightness = 10.f; //10 is good 25 is garbo
		auto Fov = 8.f; //Way too high = you will be hella sus
		auto smoothing = 1.f; //Way too low = you will be hella sus
							  //Change it to like 5
		auto bestAngle = Vector3{};
		//
		
		for (auto i = 1; i <= 32; ++i)
		{
			const auto& player = memory.Read<std::uintptr_t>(client + offset::dwEntityList + i * 0x10);
			const auto& entity = memory.Read<std::uintptr_t>(client + offset::dwEntityList + i * 0x10);
			if (memory.Read<std::int32_t>(entity + offset::m_iTeamNum) == localTeam)
			{
				memory.Write<Color>(entity + offset::m_clrRender, teamColor);
			}
			else
			{
				memory.Write<Color>(entity + offset::m_clrRender, enemyColor);
			}
			const auto _this = static_cast<std::uintptr_t>(engine + offset::model_ambient_min - 0x2c);
			memory.Write<std::int32_t>(engine + offset::model_ambient_min, *reinterpret_cast<std::uintptr_t*> (&brightness) ^ _this);
			if (!GetAsyncKeyState(VK_SHIFT))
				continue;
			if (memory.Read<std::int32_t>(player + offset::m_iTeamNum) == localTeam)
				continue;
			if (memory.Read<bool>(player + offset::m_bDormant))
				continue;
			if (!memory.Read<std::int32_t>(player + offset::m_iHealth))
				continue;
			if (!memory.Read<bool>(player + offset::m_bSpottedByMask))
				continue;

			const auto boneMatrix = memory.Read<std::uintptr_t>(player + offset::m_dwBoneMatrix);
			const auto playerHeadPosition = Vector3{
				memory.Read<float>(boneMatrix + 0x30 * 8 + 0x0c),
				memory.Read<float>(boneMatrix + 0x30 * 8 + 0x1c),
				memory.Read<float>(boneMatrix + 0x30 * 8 + 0x2c)
			};
			const auto angle = CalculateAngle(
				localEyePosition,
				playerHeadPosition,
				viewAngles + aimPunch
			);
			const auto fov = std::hypot(angle.x, angle.y);
			if (fov < Fov)
			{
				Fov = fov;
				bestAngle = angle;
			}
			if (!bestAngle.IsZero())			
				memory.Write<Vector3>(clientState + offset::dwClientState_ViewAngles, viewAngles + bestAngle / smoothing);
			
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}