#include "pch.h"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::Security::Cryptography;
using namespace winrt::Windows::Security::Credentials;

void help() {
  std::cout << "USAGE: WindowsHelloService.exe <mode> username" << std::endl;
  std::cout << "MODE" << std::endl;
  std::cout << "  enroll:       enroll a new user with Windows Hello" << std::endl;
  std::cout << "  authenticate: anthenticate the user with Windows Hello" << std::endl;
}

int enroll(std::string username) {
  if (!KeyCredentialManager::IsSupportedAsync().get()) {
    std::cerr << "Windows Hello is not supported" << std::endl;
    return 1;
  }
  auto result = KeyCredentialManager::RequestCreateAsync(
                    to_hstring(username), KeyCredentialCreationOption::ReplaceExisting)
                    .get();
  if (result.Status() == KeyCredentialStatus::Success) {
    std::cout << "Create user " << username << " successfully!" << std::endl;
    return 0;
  } else {
    std::cerr << "Creating user " << username << " failed!" << std::endl;
    return 1;
  }
}

void focus_hello_window(std::future<void> signal) {
  HWND hwnd = FindWindowW(L"Credential Dialog Xaml Host", NULL);
  while (hwnd == NULL && signal.wait_for(std::chrono::milliseconds(500)) ==
                             std::future_status::timeout) {
    hwnd = FindWindowW(L"Credential Dialog Xaml Host", NULL);
  }
  if (hwnd != NULL) {
    HWND hForeWnd = GetForegroundWindow();
    DWORD dwCurId = GetCurrentThreadId();
    DWORD dwForId = GetWindowThreadProcessId(hForeWnd, NULL);
    AttachThreadInput(dwCurId, dwForId, TRUE);
    ShowWindow(hwnd, SW_SHOWNORMAL);
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
    SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
    SetForegroundWindow(hwnd);
    AttachThreadInput(dwCurId, dwForId, FALSE);
  }
}

int authenticate(std::string username) {
  auto result = KeyCredentialManager::OpenAsync(to_hstring(username)).get();
  if (result.Status() == KeyCredentialStatus::Success) {
    auto key = result.Credential();
    uint8_t message[] = "Fuck the world if you are rich, otherwise fuck yourself.";
    auto data = CryptographicBuffer::CreateFromByteArray(message);
    std::promise<void> exit_signal;
    std::thread t(focus_hello_window, exit_signal.get_future());
    auto sign_result = key.RequestSignAsync(data).get();
    exit_signal.set_value();
    t.join();
    if (sign_result.Status() == KeyCredentialStatus::Success) {
      return 0;
    }
  }
  return 1;
}

// return 0 => success
// reutrn _ => fail
int main(int argc, char* argv[]) {

  using namespace std::literals;

  init_apartment();

  if (argc != 3) {
    help();
    return 1;
  }

  auto mode = argv[1];
  auto username = argv[2];
  if ("enroll"s == mode) {
    return enroll(username);
  } else if ("authenticate"s == mode) {
    return authenticate(username);
  } else {
    help();
    return 1;
  }
}
