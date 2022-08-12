#pragma once

#include <QFileDialog>

#if _WIN32
static constexpr const auto FILE_PICKER_OPTS = static_cast<QFileDialog::Option>( 0 );
#else
static constexpr const auto FILE_PICKER_OPTS = QFileDialog::Option::DontUseNativeDialog;
#endif
