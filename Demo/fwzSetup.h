#pragma once 
#include <stdio.h>
#include <windows.h>

typedef struct {
  HINSTANCE hInstance;
  int scrWidth;
  int scrHeight;
  int nWindowed;
} fwzSettings;

int OpenSetupDialog(fwzSettings * setup);