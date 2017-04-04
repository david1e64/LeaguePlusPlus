#include "PluginSDK.h"

PluginSetup("KennenPonche");

IMenu* MainMenu;

IMenu* ComboMenu;
IMenuOption* ComboQ;
IMenuOption* ComboW;
IMenuOption* ComboWOnly;
IMenuOption* ComboE;
IMenuOption* ComboR;

IMenu* RMenu;
IMenuOption* RRange;
IMenuOption* RAuto;
IMenuOption* RCancel;
IMenuOption* RMin;

IMenu* FleeMenu;
IMenuOption* FleeE;
IMenuOption* FleeKey;

IMenu* HarassMenu;
IMenuOption* HarassQ;
IMenuOption* HarassW;
IMenuOption* HarassWOnly;
IMenuOption* HarassE;
IMenuOption* HarassToggleKey;
IMenuOption* HarassDisabled;

IMenu* LaneClearMenu;
IMenuOption* LaneClearQ;
IMenuOption* LaneClearW;
IMenuOption* LaneClearE;
IMenuOption* LaneClearDisabled;

IMenu* KillStealMenu;
IMenuOption* KillStealQ;
IMenuOption* KillStealW;
IMenuOption* KillStealR;
IMenuOption* KillStealMin;
IMenuOption* KillStealDisabled;

IMenu* DrawMenu;
IMenu* DrawQMenu;
IMenuOption* DrawQ;
IMenuOption* DrawQColor;
IMenu* DrawWMenu;
IMenuOption* DrawW;
IMenuOption* DrawWEnabled;
IMenuOption* DrawWColor;
IMenu* DrawEMenu;
IMenuOption* DrawE;
IMenuOption* DrawEColor;
IMenu* DrawRMenu;
IMenuOption* DrawR;
IMenuOption* DrawRColor;
IMenu* DrawHarassMenu;
IMenuOption* DrawHarass;
IMenuOption* DrawHarassColor;
IMenuOption* DrawCD;
IMenuOption* DrawDisabled;

IMenu* MiscMenu;
IMenuOption* MiscERange;

ISpell2* Q;
ISpell2* W;
ISpell2* E;
ISpell2* R;

bool HarassToggle = false;
float LastChanged = 0;
IUnit *Player;


void LoadSpells()
{
	Q = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, true, false, static_cast<eCollisionFlags>(kCollidesWithMinions | kCollidesWithYasuoWall));
	W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, kCollidesWithNothing);
	E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, false, false, kCollidesWithNothing);
	R = GPluginSDK->CreateSpell2(kSlotR, kCircleCast, false, true, kCollidesWithNothing);

	Q->SetSkillshot(0.f, 50.f, 1700.f, 1050.f);
	W->SetOverrideRange(725.f);
}

void LoadMenu()
{
	auto nb_champ = 0;
	MainMenu = GPluginSDK->AddMenu("KennenPonche");

	ComboMenu = MainMenu->AddMenu("Combo Manager");
	ComboQ = ComboMenu->CheckBox("Use Q", true);
	ComboW = ComboMenu->CheckBox("Use W", true);
	ComboWOnly = ComboMenu->CheckBox("Only W If It Will Stun", false);
	ComboE = ComboMenu->CheckBox("Use E", true);
	ComboR = ComboMenu->CheckBox("Use R", true);

	RMenu = MainMenu->AddMenu("R Manager");
	RAuto = RMenu->CheckBox("Auto R", true);
	RCancel = RMenu->CheckBox("Cancel R", true);
	RRange = RMenu->AddInteger("R Range", 0, 550, 480);
	for (auto Enemy : GEntityList->GetAllHeros(false, true))
	{
		RMenu->AddInteger(Enemy->ChampionName(), 0, 3, 1);
		nb_champ++;
	}
	RMin = RMenu->AddInteger("Min to R", 1, nb_champ * 3, 1);

	FleeMenu = MainMenu->AddMenu("Flee Manager");
	FleeE = FleeMenu->CheckBox("Use E", true);
	FleeKey = FleeMenu->AddKey("Key", 84);

	HarassMenu = MainMenu->AddMenu("Harass Manager");
	HarassQ = HarassMenu->CheckBox("Use Q", true);
	HarassW = HarassMenu->CheckBox("Use W", true);
	HarassWOnly = ComboMenu->CheckBox("Only W If It Will Stun", false);
	HarassE = HarassMenu->CheckBox("Use E", false);
	HarassToggleKey = HarassMenu->AddKey("Key", 87);
	HarassDisabled = HarassMenu->CheckBox("Disable All", false);

	/*
	LaneClearMenu = MainMenu->AddMenu("LaneClear Manager");
	LaneClearQ = LaneClearMenu->CheckBox("Use Q", true);
	LaneClearW = LaneClearMenu->CheckBox("Use W", true);
	LaneClearE = LaneClearMenu->CheckBox("Use E", false);
	LaneClearDisabled = LaneClearMenu->CheckBox("Disable All", false);
	 */

	KillStealMenu = MainMenu->AddMenu("KillSteal Manager");
	KillStealQ = KillStealMenu->CheckBox("Use Q", true);
	KillStealW = KillStealMenu->CheckBox("Use W", true);
	KillStealR = KillStealMenu->CheckBox("Use R", true);
	KillStealMin = KillStealMenu->AddInteger("Min to Hit With R", 1, 5, 2);
	KillStealDisabled = KillStealMenu->CheckBox("Disable All", false);

	DrawMenu = MainMenu->AddMenu("Draw Manager");
	DrawQMenu = DrawMenu->AddMenu("Q Draw Manager");
	DrawQ = DrawQMenu->CheckBox("Enabled", true);
	DrawQColor = DrawQMenu->AddColor("Q Color Picker", 255, 255, 0, 255);
	DrawWMenu = DrawMenu->AddMenu("W Draw Manager");
	DrawW = DrawWMenu->CheckBox("Enabled", true);
	DrawWColor = DrawWMenu->AddColor("W Color Picker", 255, 255, 0, 255);
	DrawEMenu = DrawMenu->AddMenu("E Draw Manager");
	DrawE = DrawEMenu->CheckBox("Enabled", true);
	DrawEColor = DrawEMenu->AddColor("E Color Picker", 255, 255, 0, 255);
	DrawRMenu = DrawMenu->AddMenu("R Draw Manager");
	DrawR = DrawRMenu->CheckBox("Enabled", true);
	DrawRColor = DrawRMenu->AddColor("R Color Picker", 255, 255, 0, 255);
	DrawHarassMenu = DrawMenu->AddMenu("Harass Draw Manager");
	DrawHarass = DrawHarassMenu->CheckBox("Enabled", true);
	DrawHarassColor = DrawHarassMenu->AddColor("Harass Color Picker", 255, 255, 0, 255);
	DrawCD = DrawMenu->CheckBox("Disable on CD", true);
	DrawDisabled = DrawMenu->CheckBox("Disable All", false);

	MiscMenu = MainMenu->AddMenu("Misc Manager");
	MiscERange = MiscMenu->AddInteger("Min Range to E", 0, 1600, 1250);
}

PLUGIN_EVENT(void) OnRender()
{
	Vec4 Color;
	auto HeroPos = Player->GetPosition();
	Vec2 Pos;

	if (!DrawDisabled->Enabled())
	{
		if (DrawCD->Enabled())
		{
			if (Q->IsReady() && DrawQ->Enabled())
			{
				DrawQColor->GetColor(&Color);
				GRender->DrawOutlinedCircle(HeroPos, Color, Q->Range());
			}
			if (W->IsReady() && DrawW->Enabled())
			{
				DrawWColor->GetColor(&Color);
				GRender->DrawOutlinedCircle(HeroPos, Color, W->Range());
			}
			if (R->IsReady() && DrawR->Enabled())
			{
				DrawRColor->GetColor(&Color);
				GRender->DrawOutlinedCircle(HeroPos, Color, RRange->GetInteger());
			}
		}
		else
		{
			if (DrawQ->Enabled())
			{
				DrawQColor->GetColor(&Color);
				GRender->DrawOutlinedCircle(Player->GetPosition(), Color, Q->Range());
			}
			if (DrawW->Enabled())
			{
				DrawWColor->GetColor(&Color);
				GRender->DrawOutlinedCircle(HeroPos, Color, W->Range());
			}
			if (DrawR->Enabled())
			{
				DrawRColor->GetColor(&Color);
				GRender->DrawOutlinedCircle(HeroPos, Color, RRange->GetInteger());
			}
		}
		if (DrawHarass->Enabled())
		{
			DrawHarassColor->GetColor(&Color);
			if (HarassToggle)
			{
				if (Player->GetHPBarPosition(Pos))
				{
					Pos.x += 20;
					Pos.y += 50;
					GRender->DrawText(Pos, Color, "Harass Toggle Enabled");
				}
			}

		}
	}
}

int CountR()
{
	auto Count = 0;

	for (auto Enemy : GEntityList->GetAllHeros(false, true))
	{
		if (Enemy != nullptr && Enemy->IsValidObject() && Enemy->IsVisible() && Enemy->IsValidTarget(Player, RRange->GetInteger()) && !Enemy->IsDead() && !Enemy->HasBuff("sionpassivezombie"))
		{
			Count += RMenu->GetOption(Enemy->ChampionName())->GetInteger();
		}
	}
	return (Count);
}

int IsEnough(Vec3 Location, int range)
{
	Vec3 Pos;
	auto Count = 0;

	for (auto Enemy : GEntityList->GetAllHeros(false, true))
	{
		if (Enemy != nullptr && Enemy->IsValidTarget() && !Enemy->IsDead() && Enemy->IsValidObject() && Enemy->IsVisible())
		{
			GPrediction->GetFutureUnitPosition(Enemy, W->GetDelay(), true, Pos);
			if ((Pos - Location).Length2D() <= range)
			{
				Count++;
			}
		}
	}
	return (Count);
}

void CastQ(IUnit* target)
{
	Q->CastOnTarget(target, kHitChanceHigh);
}

void CastW()
{
	W->CastOnPlayer();
}

void CastE()
{
	E->CastOnPlayer();
}

void CastR()
{
	R->CastOnPlayer();
}

bool CheckW(IUnit *target)
{
	if (target != nullptr && ((target->GetBuffCount("kennenmarkofstorm") > 1 || !ComboWOnly->Enabled() && target->HasBuff("kennenmarkofstorm"))))
	{
		return (true);
	}
	for (auto Enemy : GEntityList->GetAllHeros(false, true))
	{
		if (Enemy != nullptr && Enemy->IsValidTarget(Player, W->Range()) && !Enemy->IsDead() && Enemy->IsValidObject() && Enemy->IsVisible())
		{
			if (Enemy->GetBuffCount("kennenmarkofstorm") > 1)
			{
				return (true);
			}
		}
	}
	return (false);
}

void Combo()
{
	auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());

	if (target != nullptr)
	{
		if (ComboE->Enabled() && E->IsReady() && !Player->HasBuff("KennenLightningRush") && target->IsValidTarget(Player, MiscERange->GetInteger()))
		{
			CastE();
		}
		if (ComboQ->Enabled() && Q->IsReady())
		{
			CastQ(target);
		}
	}
	if (ComboW->Enabled() && W->IsReady() && CheckW(target))
	{
		CastW();
	}
	if (ComboR->Enabled() && R->IsReady() && !RAuto->Enabled() && CountR() >= RMin->GetInteger())
	{
		CastR();
	}
}

void Harass()
{
	auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());

	if (target != nullptr)
	{
		if (HarassE->Enabled() && E->IsReady() && !Player->HasBuff("KennenLightningRush") && target->IsValidTarget(Player, MiscERange->GetInteger()))
		{
			CastE();
		}
		if (HarassQ->Enabled() && Q->IsReady())
		{
			CastQ(target);
		}
	}
	if (HarassW->Enabled() && W->IsReady() && CheckW(target))
	{
		CastW();
	}
}
/*
 void LaneClear()
{

}
 */

int GetRDelay()
{
	auto Level = Player->GetSpellLevel(kSlotR);

	if (Level == 1)
		return (3);
	else if (Level == 2)
		return (4);
	else
		return (5);
}

void KillSteal()
{
	float RDamage;

	if (!KillStealDisabled->Enabled()) 
	{ 
		for (auto Enemy : GEntityList->GetAllHeros(false, true))
		{
			if (!Enemy->IsDead() && Enemy->IsValidObject() && Enemy->IsVisible())
			{
				RDamage = GHealthPrediction->GetKSDamage(Enemy, kSlotR, GetRDelay(), false);
				if (Q->IsReady() && KillStealQ->Enabled() && Enemy->IsValidTarget(Player, Q->Range()) && GHealthPrediction->GetKSDamage(Enemy, kSlotQ, 0.f, true) > Enemy->GetHealth())
				{
					CastQ(Enemy);
				}
				if (W->IsReady() && KillStealW->Enabled() && Enemy->IsValidTarget(Player, W->Range()) && Enemy->HasBuff("kennenmarkofstorm") && GHealthPrediction->GetKSDamage(Enemy, kSlotQ, 0.25, false) > Enemy->GetHealth())
				{
					CastW();
				}
				if (R->IsReady() && KillStealR->Enabled() && Enemy->IsValidTarget(Player, RRange->GetInteger()) && GHealthPrediction->GetKSDamage(Enemy, kSlotQ, GetRDelay(), false) > Enemy->GetHealth() && CountR() >= KillStealMin->GetInteger())
				{
					CastR();
				}
			}
		}
	}
}

void Flee()
{
	GGame->IssueOrder(Player, kMoveTo, GGame->CursorPosition());
	if (E->IsReady() && FleeE->Enabled() && !Player->HasBuff("KennenLightningRush"))
	{
		CastE();
	}
}

void ChangeHarassToggle()
{
	if (HarassToggle)
	{
		HarassToggle = false;
	}
	else
	{
		HarassToggle = true;
	}
	LastChanged = GGame->Time();
}

PLUGIN_EVENT(void) OnGameUpdate()
{
	KillSteal();
	if (RAuto->Enabled() && R->IsReady() && CountR() >= RMin->GetInteger())
	{
		CastR();
	}
	 	if (GetAsyncKeyState(FleeKey->GetInteger()) && !GGame->IsChatOpen() && GUtility->IsLeagueWindowFocused())
	{
		Flee();
	}
	if (GetAsyncKeyState(HarassToggleKey->GetInteger()) && !GGame->IsChatOpen() && GUtility->IsLeagueWindowFocused() && LastChanged + 1 < GGame->Time())
	{
		ChangeHarassToggle();
	}
	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo)
	{
		Combo();
	}
	if (GOrbwalking->GetOrbwalkingMode() == kModeMixed || HarassToggle)
	{
		Harass();
	}
	/*
	if (GOrbwalking->GetOrbwalkingMode() == kModeLaneClear)
	{
	LaneClear();
	}
	 */
}

void InstallEventHandlers()
{
	GEventManager->AddEventHandler(kEventOnRender, OnRender);
	GEventManager->AddEventHandler(kEventOnGameUpdate, OnGameUpdate);
}

void UninstallEventHandlers()
{
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
}

PLUGIN_API void OnLoad(IPluginSDK* PluginSDK)
{
	PluginSDKSetup(PluginSDK);
	LoadSpells();
	InstallEventHandlers();
	Player = GEntityList->Player();
	LoadMenu();
	if (strcmp(Player->ChampionName(), "Kennen") == 0)
	{
		GGame->PrintChat("Kennen Ponche : Loaded");
	}
	else
	{
		GGame->PrintChat("Kennen Ponche : You are not playing Orianna...");
	}
}

PLUGIN_API void OnUnload()
{
	MainMenu->Remove();
	UninstallEventHandlers();
	GGame->PrintChat("Kennen Ponche : Unloaded");
}