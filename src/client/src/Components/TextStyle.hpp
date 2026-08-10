#pragma once

enum class TextSize { SMALL, MEDIUM, MEDIUM_OR_MORE, LARGE };

enum class TextStyle {
    TITLE,
    BUTTON_TEXT,
    NORMAL_TEXT,
    SUBTITLE,
    SETTINGS_LABEL,
    WARNING_TEXT,
    TAB_TITLE
};

struct TextSizeConfig {
    static unsigned int getSize(TextStyle style, TextSize size)
    {
        switch (size) {
            case TextSize::SMALL:
                switch (style) {
                    case TextStyle::TITLE:
                        return 40;
                    case TextStyle::BUTTON_TEXT:
                        return 16;
                    case TextStyle::NORMAL_TEXT:
                        return 20;
                    case TextStyle::SUBTITLE:
                        return 20;
                    case TextStyle::SETTINGS_LABEL:
                        return 16;
                    case TextStyle::WARNING_TEXT:
                        return 14;
                    case TextStyle::TAB_TITLE:
                        return 18;
                    default:
                        return 16;
                }
            case TextSize::MEDIUM:
                switch (style) {
                    case TextStyle::TITLE:
                        return 50;
                    case TextStyle::BUTTON_TEXT:
                        return 20;
                    case TextStyle::NORMAL_TEXT:
                        return 24;
                    case TextStyle::SUBTITLE:
                        return 24;
                    case TextStyle::SETTINGS_LABEL:
                        return 20;
                    case TextStyle::WARNING_TEXT:
                        return 16;
                    case TextStyle::TAB_TITLE:
                        return 22;
                    default:
                        return 20;
                }
            case TextSize::MEDIUM_OR_MORE:
                switch (style) {
                    case TextStyle::TITLE:
                        return 55;
                    case TextStyle::BUTTON_TEXT:
                        return 22;
                    case TextStyle::NORMAL_TEXT:
                        return 26;
                    case TextStyle::SUBTITLE:
                        return 26;
                    case TextStyle::SETTINGS_LABEL:
                        return 22;
                    case TextStyle::WARNING_TEXT:
                        return 18;
                    case TextStyle::TAB_TITLE:
                        return 24;
                    default:
                        return 22;
                }
            case TextSize::LARGE:
                switch (style) {
                    case TextStyle::TITLE:
                        return 60;
                    case TextStyle::BUTTON_TEXT:
                        return 24;
                    case TextStyle::NORMAL_TEXT:
                        return 28;
                    case TextStyle::SUBTITLE:
                        return 28;
                    case TextStyle::SETTINGS_LABEL:
                        return 24;
                    case TextStyle::WARNING_TEXT:
                        return 20;
                    case TextStyle::TAB_TITLE:
                        return 26;
                    default:
                        return 24;
                }
            default:
                return 20;
        }
    }
};
