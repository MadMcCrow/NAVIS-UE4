// Noe Perard-Gayot <noe.perard@gmail.com> 2019 - All Rights Reserved
#include "NAVIS_CustomMesh.h"

DEFINE_LOG_CATEGORY(LogNAVIS_CustomMesh);

#define LOCTEXT_NAMESPACE "NAVIS_CustomMesh"

void FNAVIS_CustomMesh::StartupModule()
{
	UE_LOG(LogNAVIS_CustomMesh, Warning, TEXT("NAVIS_CustomMesh module has started"));
}

void FNAVIS_CustomMesh::ShutdownModule()
{
	UE_LOG(LogNAVIS_CustomMesh, Warning, TEXT("NAVIS_CustomMesh module has shut down"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FNAVIS_CustomMesh, NAVIS_CustomMesh)