// Noe Perard-Gayot <noe.perard@gmail.com> 2019 - All Rights Reserved
#include "NAVIS_Types.h"

DEFINE_LOG_CATEGORY(LogNAVIS_Types);

#define LOCTEXT_NAMESPACE "NAVIS_Types"

void FNAVIS_Types::StartupModule()
{
	UE_LOG(LogNAVIS_Types, Warning, TEXT("NAVIS_Types module has started"));
}

void FNAVIS_Types::ShutdownModule()
{
	UE_LOG(LogNAVIS_Types, Warning, TEXT("NAVIS_Types module has shut down"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FNAVIS_Types, NAVIS_Types)