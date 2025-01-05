#include "framework.h"
#include "Navigation.h"
#include "InstallerEngine.h"
#include "DirectUI/DirectUI.h"

using namespace std;
using namespace DirectUI;

vector<int> visited;
vector<int> visitedwidth;

IEngineWrapper animThread;
IEngineWrapper mainThread;

HRESULT errf;

int ChangePage(int target) {
	if (curr >= 1) {
		if (pageArr[curr]) {
			NavLogger.WriteLine(L"hiding page " + to_wstring(curr));
			pageArr[curr]->SetVisible(false);
		}
		else {
			NavLogger.WriteLine(L"unspecified errfor inside page: " + to_wstring(curr));
		}

		if (animArr[curr]) {
			NavLogger.WriteLine(L"setting animator" + to_wstring(curr) + L" width to 0");
			animArr[curr]->SetWidth(0);
		}
		else {
			NavLogger.WriteLine(L"unspecified error inside page: " + to_wstring(curr) + L". Please check whether all the resources are valid or not");
		}
	}
	if (target >= 1) {
		if (pageArr[target]) {
			NavLogger.WriteLine(L"unhiding page " + to_wstring(target));
			pageArr[target]->SetVisible(true);
		}
		else {
			NavLogger.WriteLine(L"unspecified error inside page: " + to_wstring(target) + L". Please check whether all the resources are valid or not");
		}

		if (animArr[target]) {
			NavLogger.WriteLine(L"setting animator" + to_wstring(target) + L" width to 50. Animation plays.");
			animArr[target]->SetWidth(50);
		}
		else {
			NavLogger.WriteLine(L"unspecified error inside page: " + to_wstring(target) + L". Please check whether all the resources are valid or not");
		}
	}
	return target;
}

void TaskAtArrival() {
	NavLogger.WriteLine(L"TaskAtArrival() called at page " + to_wstring(curr));
	nxt=curr+1;
	if (!uninstall) {
		if (curr == DEFENDERPAGE) {
			if (Bck) {
				NavLogger.WriteLine(L"Changing back button content string to 'Exit'");
				Bck->SetContentString((UCString)L"Exit");
			}
			else {
				NavLogger.WriteLine(L"Unspecified errfor. Please check validity of duixml");
			}
		}
		else {
			if (Bck) {
				NavLogger.WriteLine(L"Changing back button content string to 'Back'");
				Bck->SetContentString((UCString)L"Back");
			}
			else {
				NavLogger.WriteLine(L"Unspecified errfor. Please check validity of duixml");
			}
		}
		if (curr == PATCHSELECTPAGE || curr == INSTALLSELECTPAGE) {
			if (Nxt) {
				NavLogger.WriteLine(L"Changing next button layoutpos to -3");
				Nxt->SetLayoutPos(-3);
			}
			else {
				NavLogger.WriteLine(L"Unspecified errfor. Please check validity of duixml");
			}
		}
		else {
			if (Nxt) {
				NavLogger.WriteLine(L"Changing next button layoutpos to 0");
				Nxt->SetLayoutPos(0);
			}
			else {
				NavLogger.WriteLine(L"Unspecified errfor. Please check validity of duixml");
			}
		}
		if (curr == PROGRESSPAGE) {
			if (Bck) {
				NavLogger.WriteLine(L"Hiding Back button");
				Bck->SetVisible(false);
			}
			else {
				NavLogger.WriteLine(L"Unspecified errfor. Please check validity of duixml");
			}
			if (Nxt) {
				NavLogger.WriteLine(L"Hiding next button");
				Nxt->SetVisible(false);
			}
			else {
				NavLogger.WriteLine(L"Unspecified errfor. Please check validity of duixml");
			}

			NavLogger.WriteLine(L"Starting Main Loading Animation");
			animThread.StartThread(&IEngineWrapper::BeginMainAnim);

			NavLogger.WriteLine(L"Starting Installation");
			NavLogger.WriteLine(L"\nInstallation log can be found in the file Installation.log\n");
			mainThread.StartThread(&IEngineWrapper::BeginInstall);
		}
		if (curr == RESTARTPAGE) {
			if (Nxt) {
				NavLogger.WriteLine(L"Unhiding next button and setting its content string to 'Restart now'");
				Nxt->SetVisible(true);
				Nxt->SetContentString((UCString)L"Restart now");
			}
			else {
				NavLogger.WriteLine(L"Unspecified errfor. Please check validity of duixml");
			}
			if (progressbar) {
				progressbar->SetWidth(384);
			}
			NavLogger.WriteLine(L"Starting Restart countdown Loading Animation");
			animThread.StartThread(&IEngineWrapper::BeginRestartAnim);

			NavLogger.WriteLine(L"Starting Restart countdown");
			mainThread.StartThread(&IEngineWrapper::BeginRestartCountdown);
		}
	}
	else {
		if (curr == (UNINSTALLSELECTPAGE - MAXPAGE) + 1) {
			if (Bck) {
				NavLogger.WriteLine(L"Changing back button content string to 'Exit'");
				Bck->SetContentString((UCString)L"Exit");
			}
			else {
				NavLogger.WriteLine(L"Unspecified errfor. Please check validity of duixml");
			}
		}
		if (curr == (UNINSTALLPROGRESSPAGE - MAXPAGE) + 1) {
			if (Bck) {
				NavLogger.WriteLine(L"Hiding Back button");
				Bck->SetVisible(false);
			}
			else {
				NavLogger.WriteLine(L"Unspecified errfor. Please check validity of duixml");
			}
			if (Nxt) {
				NavLogger.WriteLine(L"Hiding next button");
				Nxt->SetVisible(false);
			}
			else {
				NavLogger.WriteLine(L"Unspecified errfor. Please check validity of duixml");
			}

			NavLogger.WriteLine(L"Starting Main Loading Animation");
			animThread.StartThread(&IEngineWrapper::BeginMainAnim);

			NavLogger.WriteLine(L"Starting Installation");
			NavLogger.WriteLine(L"\nInstallation log can be found in the file Installation.log\n");
			mainThread.StartThread(&IEngineWrapper::BeginUninstall);
		}
		if (curr == (UNINSTALLRESTARTPAGE - MAXPAGE) + 1) {
			if (Nxt) {
				NavLogger.WriteLine(L"Unhiding next button and setting its content string to 'Restart now'");
				Nxt->SetVisible(true);
				Nxt->SetContentString((UCString)L"Restart now");
			}
			else {
				NavLogger.WriteLine(L"Unspecified errfor. Please check validity of duixml");
			}
			if (progressbar) {
				progressbar->SetWidth(384);
			}
			NavLogger.WriteLine(L"Starting Restart countdown Loading Animation");
			animThread.StartThread(&IEngineWrapper::BeginRestartAnim);

			NavLogger.WriteLine(L"Starting Restart countdown");
			mainThread.StartThread(&IEngineWrapper::BeginRestartCountdown);
		}
	}

}
void TaskAtLeave() {
	NavLogger.WriteLine(L"\nTaskAtLeave() called at page " + to_wstring(curr));
	if (!uninstall) {
		if (curr == PROGRESSPAGE) {
			NavLogger.WriteLine(L"Stopping all threads");
			animThread.StopThread();
			mainThread.StopThread();
		}
		if (curr == RESTARTPAGE) {
			NavLogger.WriteLine(L"Stopping animation threads and exit");
			animThread.StopThread();
			IEngineWrapper::Ttime.store(0);
		}
	}
	else {
		if (curr == (UNINSTALLPROGRESSPAGE - MAXPAGE) + 1) {
			NavLogger.WriteLine(L"Stopping all threads");
			animThread.StopThread();
			mainThread.StopThread();
		}
		if (curr == (UNINSTALLRESTARTPAGE - MAXPAGE) + 1) {
			NavLogger.WriteLine(L"Stopping animation threads and exit");
			animThread.StopThread();
			IEngineWrapper::Ttime.store(0);
		}
	}
}
void Navigate() {
	TaskAtLeave();
	NavLogger.WriteLine(L"Starting Navigation from page " + to_wstring(curr) + L" to " + to_wstring(nxt));
	if ((curr < MAXPAGE-1 && !uninstall) || (curr < MAXUNINSTPAGE-1 && uninstall)) {
		curr = ChangePage(nxt);
		if (progressbar) {
			NavLogger.WriteLine(L"Setting progress bar width to " + to_wstring(curr * 15 + progressbar->GetWidth()));
			errf = progressbar->SetWidth(curr * 15 + progressbar->GetWidth());
			if (FAILED(errf)) {
				NavLogger.WriteLine(L"Failed to set progress bar width. Please check the validity of the installer's duixml.", errf);
			}
		}
		visited.push_back(curr);
		visitedwidth.push_back(progressbar->GetWidth());
		TaskAtArrival();
	}
}
void NavigateBack() {
	TaskAtLeave();
	if (curr > 1) {
		visited.pop_back();
		visitedwidth.pop_back();
		NavLogger.WriteLine(L"Starting Navigation from page " + to_wstring(curr) + L" to " + to_wstring(visited[visited.size() - 1]));
		curr = ChangePage(visited[visited.size()-1]);
		if (progressbar) {
			NavLogger.WriteLine(L"Setting progress bar width to " + to_wstring(visitedwidth[visitedwidth.size() - 1]));
			errf = progressbar->SetWidth(visitedwidth[visitedwidth.size() - 1]);
			if (FAILED(errf)) {
				NavLogger.WriteLine(L"Failed to set progress bar width. Please check the validity of the installer's duixml.", errf);
			}
		}
		TaskAtArrival();
	}
}
