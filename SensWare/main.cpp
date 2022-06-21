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

__declspec(align(16)) struct Colorium
{
	constexpr Colorium(const float r, const float g, const float b, const float a = 1.f) noexcept :
		r(r), g(g), b(b), a(a) { }

	float r, g, b, a;
};


int main()
{
	bool takesconfig = false; //This is my configuration! │ Aimbot = M4 │ Flat Chams Disabled │ Team Chams = Green │ Enemy Chams = Pink & Red │ Bhop Enagled
	if (!takesconfig)
	std::cout << "Welcome to SensWare!\nAimbot key = SHIFT\nVisuals are visible only\nVersion:1.1";
	if (takesconfig)
	std::cout << "Welcome to SensWare!\nAimbot key = M4\nVisuals are visible only\nVersion:1.1";
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
		bool flatchams = true; //IT IS IN BETA!
		bool bhop = false;

		
		auto bestAngle = Vector3{};
		//

		for (auto i = 1; i <= 32; ++i)
		{
			constexpr const auto color = Colorium{ 1.f, 0.f, 1.f };

			const auto& player = memory.Read<std::uintptr_t>(client + offset::dwEntityList + i * 0x10);
			const auto& entity = memory.Read<std::uintptr_t>(client + offset::dwEntityList + i * 0x10);
			if (takesconfig)
			{
				if (memory.Read<std::int32_t>(entity + offset::m_iTeamNum) == localTeam)
				{
					constexpr const auto taketeamColor = Color{ 51, 255, 5 };
					memory.Write<Color>(entity + offset::m_clrRender, taketeamColor);
				}
				else
				{
					constexpr const auto takeenemyColor = Color{ 255, 5, 109 };
					memory.Write<Color>(entity + offset::m_clrRender, takeenemyColor);
				}
			}
			if (!takesconfig)
			{
				if (memory.Read<std::int32_t>(entity + offset::m_iTeamNum) == localTeam)
				{
					
					memory.Write<Color>(entity + offset::m_clrRender, teamColor);

				}
				else
				{

					memory.Write<Color>(entity + offset::m_clrRender, enemyColor);
				}
			}
		;
			
			const auto _this = static_cast<std::uintptr_t>(engine + offset::model_ambient_min - 0x2c);
			float takebright = 6.f;
			if (takesconfig)
				memory.Write<std::int32_t>(engine + offset::model_ambient_min, *reinterpret_cast<std::uintptr_t*> (&takebright) ^ _this);
			if (!takesconfig)
			{
				if (flatchams)
				{
					float flacham = 999.f;
					memory.Write<std::int32_t>(engine + offset::model_ambient_min, *reinterpret_cast<std::uintptr_t*> (&flacham) ^ _this);
				}
				if (!flatchams)
				{
					memory.Write<std::int32_t>(engine + offset::model_ambient_min, *reinterpret_cast<std::uintptr_t*> (&brightness) ^ _this);
				}
			}
			const auto localPlayerFlags = memory.Read<std::uintptr_t>(localPlayer + offset::m_fFlags);
			if (takesconfig)
			{ 
			if (GetAsyncKeyState(VK_SPACE))
			{
				(localPlayerFlags & (1 << 0)) ?
					memory.Write<std::uintptr_t>(client + offset::dwForceJump, 6) :
					memory.Write<std::uintptr_t>(client + offset::dwForceJump, 4);
			}
			const auto entityu = memory.Read<std::uintptr_t>(client + offset::dwEntityList + i * 0x10);

			if (!entity)
				continue;

			if (memory.Read<std::uintptr_t>(entityu + offset::m_iTeamNum) == offset::m_iTeamNum)
				continue;

			const auto glowIndex = memory.Read<std::int32_t>(entityu + offset::m_iGlowIndex);

			memory.Write<Colorium>(offset::dwGlowObjectManager + (glowIndex * 0x38) + 0x8, color);

			memory.Write<bool>(offset::dwGlowObjectManager + (glowIndex * 0x38) + 0x28, true);
			memory.Write<bool>(offset::dwGlowObjectManager + (glowIndex * 0x38) + 0x29, false);
			}
			if (!takesconfig)
			{
				if (bhop)
				{
					if (GetAsyncKeyState(VK_SPACE))
					{
						(localPlayerFlags & (1 << 0)) ?
							memory.Write<std::uintptr_t>(client + offset::dwForceJump, 6) :
							memory.Write<std::uintptr_t>(client + offset::dwForceJump, 4);
					}
					const auto entityu = memory.Read<std::uintptr_t>(client + offset::dwEntityList + i * 0x10);

					if (!entity)
						continue;

					if (memory.Read<std::uintptr_t>(entityu + offset::m_iTeamNum) == offset::m_iTeamNum)
						continue;

					const auto glowIndex = memory.Read<std::int32_t>(entityu + offset::m_iGlowIndex);

					memory.Write<Colorium>(offset::dwGlowObjectManager + (glowIndex * 0x38) + 0x8, color);

					memory.Write<bool>(offset::dwGlowObjectManager + (glowIndex * 0x38) + 0x28, true);
					memory.Write<bool>(offset::dwGlowObjectManager + (glowIndex * 0x38) + 0x29, false);
				}
			}
			if (takesconfig)
			{ 
			if (!GetAsyncKeyState(VK_XBUTTON2))
				continue;
			}
			if (!takesconfig)
			{
				if (!GetAsyncKeyState(VK_SHIFT))
					continue;
			}
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
			if (!takesconfig)
			{
			if (fov < Fov)
			{
				Fov = fov;
				bestAngle = angle;
			}
			}
			if (takesconfig)
			{
				auto sussyfov = 5.f;
				if (fov < sussyfov)
				{
					sussyfov = fov;
					bestAngle = angle;
				}
			}
			if (!bestAngle.IsZero())
				if (!takesconfig)
				memory.Write<Vector3>(clientState + offset::dwClientState_ViewAngles, viewAngles + bestAngle / smoothing);
				if (takesconfig)
				memory.Write<Vector3>(clientState + offset::dwClientState_ViewAngles, viewAngles + bestAngle / 3.f);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}