#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"
#include <winrt/windows.ui.text.h>
#include <TOM.h>

std::stringstream boxText;
winrt::Windows::Storage::Streams::IRandomAccessStream stream;

using namespace winrt;
using namespace Windows::UI;
using namespace Xaml;
using namespace Text;

namespace winrt::Algebra_System_3::implementation
{
    MainPage* main_page = nullptr;

    inline Algebra_System_3::implementation::MainPage::MainPage()
    {
        main_page = this;
        // Xaml objects should not call InitializeComponent during construction.
        // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        this->AddHandler(winrt::Windows::UI::Xaml::UIElement::KeyDownEvent(),
            winrt::box_value(winrt::Windows::UI::Xaml::Input::KeyEventHandler(this, &MainPage::MainBox_KeyDown)), true);
   
        Color col{};
        col.A = 51;
        col.R = 255;
        col.G = 255;
        col.B = 255;
        Clicked = Xaml::Style(xaml_typename<Controls::Button>());
        Clicked.Setters().Append(Setter(Xaml::Controls::Control::BackgroundProperty(), winrt::box_value(Colors::Gray())));

        Unclicked = Xaml::Style(xaml_typename<Controls::Button>());
        Unclicked.Setters().Append(Setter(Xaml::Controls::Control::BackgroundProperty(), winrt::box_value(col/*Colors::DarkGray()*/)));
    }

    int32_t MainPage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void MainPage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void MainPage::ClickHandler(IInspectable const&, RoutedEventArgs const&)
    {       
    }
    
    void MainPage::Button_Click(Windows::Foundation::IInspectable const& sender, RoutedEventArgs const& e)
    {        
        auto label = sender.as<Controls::Button>().Name();
        if (label == L"root2")
            MainBox().TextDocument().Selection().TypeText(L"√");
        else if (label == L"root3")
            MainBox().TextDocument().Selection().TypeText(L"∛");
        else if (label == L"root4")
            MainBox().TextDocument().Selection().TypeText(L"∜");
        else
        {
            wchar_t C = *(label.begin());

            if (Caps().IsOn() && C > '03B0' && C < '03CA')
                C -= 32;
            MainBox().TextDocument().Selection().TypeText(std::wstring({C}));
        }
        MainBox().Focus(FocusState::Programmatic);
    }

    void MainPage::MainBox_TextChanged(IInspectable const& sender, RoutedEventArgs const& e)
    {           
    }

    void MainPage::MainBox_KeyDown(IInspectable const& sender, Xaml::Input::KeyRoutedEventArgs const& e)
    {
        if (e.OriginalKey() == Windows::System::VirtualKey::Enter)
        {
            std::wstringstream WS;
            charProps lastProps;
            hstring temp;
            MainBox().TextDocument().Selection().MoveLeft(TextRangeUnit::Character, 1, false);
            MainBox().TextDocument().Selection().StartOf(TextRangeUnit::Line, false);
            MainBox().TextDocument().Selection().MoveEnd(TextRangeUnit::Character, 1);
                        
            bool lineEnd = false;
            while (!lineEnd)
            {
                MainBox().TextDocument().Selection().GetText(TextGetOptions::None, temp);
                wchar_t C = *(temp.begin());
                if (C == L'\r')
                {
                    lineEnd = true;
                    break;
                }
                charProps props(MainBox().TextDocument().Selection().CharacterFormat());
                if (!lastProps.bold && props.bold)
                    WS << "(B)";
                if (lastProps.bold && !props.bold)
                    WS << "(!B)";
                if (!lastProps.underline && props.underline)
                    WS << "(U)";
                if (lastProps.underline && !props.underline)
                    WS << "(!U)";
                if (!lastProps.superscript && props.superscript)
                    WS << "^(";
                if (lastProps.superscript && !props.superscript)
                    WS << ")";
                if (!lastProps.subscript && props.subscript)
                    WS << "_(";
                if (lastProps.subscript && !props.subscript)
                    WS << ")";
                WS << temp.begin();
                lastProps = props;

                MainBox().TextDocument().Selection().MoveEnd(TextRangeUnit::Character, 1);
                MainBox().TextDocument().Selection().MoveStart(TextRangeUnit::Character, 1);
            }

            MainBox().TextDocument().Selection().EndOf(TextRangeUnit::Window, false);

            algebra::analyse(WS);

            auto format = MainBox().TextDocument().GetDefaultCharacterFormat();
            MainBox().TextDocument().Selection().CharacterFormat(format);
        }
    }

    void MainPage::WriteOutput(std::vector<std::pair<wchar_t, charProps>> const& output)
    {
        Text::ITextParagraphFormat pFormat = MainBox().TextDocument().Selection().ParagraphFormat();
        for (auto it = output.begin(); it != output.end(); it++)
        {
            MainBox().TextDocument().Selection().TypeText(std::wstring({ it->first }));
            MainBox().TextDocument().Selection().MoveStart(TextRangeUnit::Character, -1);
            auto format = MainBox().TextDocument().GetDefaultCharacterFormat();
            if (it->second.bold)
                format.Bold(FormatEffect::On);
            if (it->second.underline)
                format.Underline(UnderlineType::Single);
            if (it->second.subscript)
                format.Subscript(FormatEffect::On);
            if (it->second.superscript)
                format.Superscript(FormatEffect::On);
            MainBox().TextDocument().Selection().CharacterFormat(format);
            MainBox().TextDocument().Selection().EndOf(TextRangeUnit::Line, false);
        }

        pFormat.Alignment(ParagraphAlignment::Center);
        MainBox().TextDocument().Selection().ParagraphFormat() = pFormat;
        MainBox().TextDocument().Selection().TypeText(L"\r");
        pFormat.Alignment(ParagraphAlignment::Left);
        MainBox().TextDocument().Selection().ParagraphFormat() = pFormat;
        MainBox().TextDocument().Selection().TypeText(L" ");
        MainBox().TextDocument().Selection().MoveStart(TextRangeUnit::Character, -1);
        MainBox().TextDocument().Selection().Delete(TextRangeUnit::Character, 1);

        Super().Style(Unclicked);
        Sub().Style(Unclicked);
        script = 0;
    }

    void MainPage::Sub_Click(IInspectable const& sender, RoutedEventArgs const& e)
    {
        script = script == -1 ? 0 : -1;
        if (script == -1)
        {
            Sub().Style(Clicked);
            Super().Style(Unclicked);
        }
        else
            Sub().Style(Unclicked);

        auto format = MainBox().TextDocument().GetDefaultCharacterFormat();
        if (script == -1)
            format.Subscript(FormatEffect::On);

        MainBox().TextDocument().Selection().CharacterFormat(format);
        MainBox().Focus(FocusState::Programmatic);
    }

    void MainPage::Super_Click(IInspectable const& sender, RoutedEventArgs const& e)
    {
        script = script == 1 ? 0 : 1;
        if (script == 1)
        {
            Super().Style(Clicked);
            Sub().Style(Unclicked);
        }
        else
            Super().Style(Unclicked);

        auto format = MainBox().TextDocument().GetDefaultCharacterFormat();
        if (script == 1)                  
            format.Superscript(FormatEffect::On);

        MainBox().TextDocument().Selection().CharacterFormat(format);
        MainBox().Focus(FocusState::Programmatic);
    }
}

