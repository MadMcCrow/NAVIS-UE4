// Noe Perard-Gayot <noe.perard@gmail.com> 2019 - All Rights Reserved
#include "NAVIS_Physics.h"

DEFINE_LOG_CATEGORY(LogNAVIS_Physics);

#define LOCTEXT_NAMESPACE "NAVIS_Physics"

void FNAVIS_Physics::StartupModule()
{
	UE_LOG(LogNAVIS_Physics, Warning, TEXT("NAVIS_Physics module has started"));
}

void FNAVIS_Physics::ShutdownModule()
{
	UE_LOG(LogNAVIS_Physics, Warning, TEXT("NAVIS_Physics module has shut down"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FNAVIS_Physics, NAVIS_Physics)