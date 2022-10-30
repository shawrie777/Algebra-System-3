#pragma once

#include "MainPage.g.h"
#include <sstream>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <TOM.h>

namespace winrt::Algebra_System_3::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void ClickHandler(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
        void Button_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        void MainBox_TextChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
 
        void MainBox_KeyDown(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::Input::KeyRoutedEventArgs const& e);
        void WriteOutput(std::vector<std::pair<wchar_t, charProps>> const& output);        

        int script = 0;
        void Sub_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        void Super_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);

        winrt::Windows::UI::Xaml::Style Clicked;
        winrt::Windows::UI::Xaml::Style Unclicked;
    };

    extern MainPage* main_page;
}

namespace winrt::Algebra_System_3::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
