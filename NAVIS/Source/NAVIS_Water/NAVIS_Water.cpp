// Noe Perard-Gayot <noe.perard@gmail.com> 2019 - All Rights Reserved
#include "NAVIS_Water.h"

DEFINE_LOG_CATEGORY(LogNAVIS_Water);

#define LOCTEXT_NAMESPACE "NAVIS_Water"

void FNAVIS_Water::StartupModule()
{
	UE_LOG(LogNAVIS_Water, Warning, TEXT("NAVIS_Water module has started"));
}

void FNAVIS_Water::ShutdownModule()
{
	UE_LOG(LogNAVIS_Water, Warning, TEXT("NAVIS_Water module has shut down"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FNAVIS_Water, NAVIS_Water)