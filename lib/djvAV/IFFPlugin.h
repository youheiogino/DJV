//------------------------------------------------------------------------------
// Copyright (c) 2008-2009 Mikael Sundell
// Copyright (c) 2004-2018 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#pragma once

#include <djvGraphics/IFF.h>

namespace djv
{
    namespace Graphics
    {
        //! This plugin provides support for the Generic Interchange File Format (IFF).
        //!
        //! File extensions: .iff, .z
        //!
        //! Supported features:
        //!
        //! - 8-bit, 16-bit, Luminance, Luminance Alpha, RGB, RGBA
        //! - File compression
        //!
        //! References:
        //!
        //! - Affine Toolkit (Thomas E. Burge), riff.h and riff.c
        //!   http://affine.org
        //! - Autodesk Maya documentation, "Overview of Maya IFF"
        //!
        //! Implementation:
        //!
        //! - Mikael Sundell, mikael.sundell@gmail.com
        class IFFPlugin : public ImageIO
        {
        public:
            explicit IFFPlugin(const QPointer<Core::CoreContext> & context);

            QString pluginName() const override;
            QStringList extensions() const override;

            QStringList option(const QString &) const override;
            bool setOption(const QString &, QStringList &) override;
            QStringList options() const override;

            void commandLine(QStringList &) override;
            QString commandLineHelp() const override;

            std::unique_ptr<ImageLoad> createLoad(const Core::FileInfo &) const override;
            std::unique_ptr<ImageSave> createSave(const Core::FileInfo &, const ImageIOInfo &) const override;

        private:
            IFF::Options _options;
        };

    } // namespace Graphics
} // namespace djv