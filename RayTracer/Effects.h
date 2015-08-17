#pragma once
class Effects
{
public:
	Effects();
	~Effects();

	// Control enable/disable of the effects
	void controlEffects();

	bool enableShadow;
	bool enableGlossyReflection;
	bool enableRefraction;
};

