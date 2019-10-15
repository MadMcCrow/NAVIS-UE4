// Noe Perard-Gayot <noe.perard@gmail.com> 2019 - All Rights Reserved

#pragma once

#include "ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogNAVIS_Water, All, All);

class FNAVIS_Water : public IModuleInterface
{
public:

	/* This will get called when the editor loads the module */
	virtual void StartupModule() override;

	/* This will get called when the editor unloads the module */
	virtual void ShutdownModule() override;
};