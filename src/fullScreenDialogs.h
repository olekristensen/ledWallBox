#pragma once
#include "ofSystemUtils.h"

ofFileDialogResult fullScreenLoadDialog(string windowTitle="", bool bFolderSelection = false, string defaultPath="");
ofFileDialogResult fullScreenSaveDialog(string defaultName, string messageName);
