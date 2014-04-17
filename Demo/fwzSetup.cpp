
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "fwzSetup.h"
#include "resource.h"

fwzSettings * setupcfg;

typedef struct {
  int w,h;
} RES;

/*
RES ress[]={
  { 320, 240},
  { 400, 300},
  { 512, 384},
  { 640, 480},
  { 800, 600},
  {1024, 768},
  {1152, 864},
  {1280, 960},
  {1600,1200},
};
*/
int nRes = 0;
RES ress[4096];

BOOL CALLBACK DlgFunc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
  switch ( uMsg ) {
    case WM_INITDIALOG: {
      int i=0;

      DEVMODE d;
      EnumDisplaySettings(NULL,0,&d);
      while(1) {
        BOOL h = EnumDisplaySettings(NULL,i++,&d);
        if (!h || nRes>4000) break;
//        if ((d.dmPelsWidth * 3) / 4 != d.dmPelsHeight) continue;
        if (d.dmDisplayOrientation != DMDO_DEFAULT) continue;
        if (d.dmBitsPerPel != 32) continue;
        if (!nRes || ress[nRes-1].w != d.dmPelsWidth || ress[nRes-1].h != d.dmPelsHeight) {
          ress[nRes].w = d.dmPelsWidth;
          ress[nRes].h = d.dmPelsHeight;
          nRes++;
          TCHAR s[500];
          _sntprintf(s,500,_T("%d * %d"),d.dmPelsWidth,d.dmPelsHeight);
          if (d.dmPelsWidth / (float)d.dmPelsHeight ==  4.0 /  3.0) _tcsncat(s,_T(" (4:3)"),500);
          if (d.dmPelsWidth / (float)d.dmPelsHeight ==  5.0 /  4.0) _tcsncat(s,_T(" (5:4)"),500);
          if (d.dmPelsWidth / (float)d.dmPelsHeight == 16.0 /  9.0) _tcsncat(s,_T(" (16:9)"),500);
          if (d.dmPelsWidth / (float)d.dmPelsHeight == 16.0 / 10.0) _tcsncat(s,_T(" (16:10)"),500);
          SendDlgItemMessage(hWnd, IDC_RESOLUTION, CB_ADDSTRING, 0, (LPARAM)s);
        }
      }
      
/*
for (i=0; i<sizeof(ress)/sizeof(RES); i++) {
        sprintf(s,"%d * %d",ress[i].w,ress[i].h);
        SendDlgItemMessage(hWnd, IDC_RESOLUTION, CB_ADDSTRING, 0, (LPARAM)s);
      }
*/
      int n = nRes - 1;

//       for (i=0; i<nRes; i++)
//         if (ress[i].w==setupcfg->scrWidth && ress[i].h==setupcfg->scrHeight)
//           n = i;

      SendDlgItemMessage(hWnd, IDC_RESOLUTION, CB_SETCURSEL, n, 0);
/*
      SendDlgItemMessage(hWnd, IDC_MULTISAMPLE, CB_ADDSTRING, 0, (LPARAM)"None");
      for (i=2; i<=8; i++) {
        sprintf(s,"%d samples",i);
        SendDlgItemMessage(hWnd, IDC_MULTISAMPLE, CB_ADDSTRING, 0, (LPARAM)s);
      }
      SendDlgItemMessage(hWnd, IDC_MULTISAMPLE, CB_SETCURSEL, 0, 0);
*/

      if (!setupcfg->nWindowed) {
        SendDlgItemMessage(hWnd, IDC_FULLSCREEN, BM_SETCHECK, 1, 1);
        EnableWindow(GetDlgItem(hWnd,IDC_ONTOP),0);
      }
/*
      if (setupcfg->nVsync)
        SendDlgItemMessage(hWnd, IDC_VSYNC, BM_SETCHECK, 1, 1);
      if (setupcfg->nMultisample)
        SendDlgItemMessage(hWnd, IDC_MULTI, BM_SETCHECK, 1, 1);
*/

//       TCHAR sz[64];
//       _sntprintf(sz,64,_T("%d"),setupcfg->nRandomSeed);
//       SetDlgItemText(hWnd, IDC_RANDOMSEED, sz);

    } break;
    case WM_COMMAND:
    switch( LOWORD(wParam) ) {
      case IDOK: {
        setupcfg->scrWidth = ress[SendDlgItemMessage(hWnd, IDC_RESOLUTION, CB_GETCURSEL, 0, 0)].w;
        setupcfg->scrHeight= ress[SendDlgItemMessage(hWnd, IDC_RESOLUTION, CB_GETCURSEL, 0, 0)].h;
        setupcfg->nWindowed    =!SendDlgItemMessage(hWnd, IDC_FULLSCREEN, BM_GETCHECK , 0, 0);
//         setupcfg->nAlwaysOnTop = SendDlgItemMessage(hWnd, IDC_ONTOP,      BM_GETCHECK , 0, 0);
//         setupcfg->nVsync       = SendDlgItemMessage(hWnd, IDC_VSYNC,      BM_GETCHECK , 0, 0);
//         setupcfg->nMultisample = SendDlgItemMessage(hWnd, IDC_MULTI,      BM_GETCHECK, 0, 0);
//        setupcfg->nRandomSeed = GetWindowLong(GetDlgItem(hWnd, IDC_RANDOMSEED),);
//         TCHAR sz[64];
//         GetDlgItemText(hWnd, IDC_RANDOMSEED, sz, 64);
//         _stscanf(sz,_T("%d"),&setupcfg->nRandomSeed); 

        //if (setupcfg->multisample>0) setupcfg->multisample++;
        EndDialog (hWnd, TRUE);
      } break;
      case IDCANCEL: {
        EndDialog (hWnd, FALSE);
      } break;
/*
      case IDC_FULLSCREEN: {
        if (SendDlgItemMessage(hWnd, IDC_FULLSCREEN, BM_GETCHECK , 0, 0)) {
          SendDlgItemMessage(hWnd, IDC_ONTOP, BM_SETCHECK, 0, 0);
          EnableWindow(GetDlgItem(hWnd,IDC_ONTOP),0);
        } else {
          EnableWindow(GetDlgItem(hWnd,IDC_ONTOP),1);
        }
      } break;
*/
    } break;
  }
  return ( WM_INITDIALOG == uMsg ) ? TRUE : FALSE;
}

int OpenSetupDialog(fwzSettings * s) {
  setupcfg = s;
  return DialogBox(s->hInstance,MAKEINTRESOURCE(IDD_SETUP),GetForegroundWindow(),DlgFunc);
}