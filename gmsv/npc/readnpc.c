#include <stdio.h>
#include <string.h>

#include "version.h"

#include "common.h"
#include "npccreate.h"
#include "npctemplate.h"

BOOL NPC_readNPCSettingFiles(char *topdirectory, int templatesize,
                             int createsize) {
  print("\n");
  if (NPC_readNPCTemplateFiles(topdirectory, templatesize) == FALSE)
    return FALSE;

  if (NPC_readNPCCreateFiles(topdirectory, createsize) == FALSE)
    return FALSE;

  return TRUE;
}
