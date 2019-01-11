//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvCore/Event.h>
#include <djvCore/ISystem.h>

namespace djv
{
    namespace Core
    {
        class IObject;

        namespace Event
        {
            class IEventLoop : public std::enable_shared_from_this<IEventLoop>
            {
                DJV_NON_COPYABLE(IEventLoop);

            protected:
                void _init(const std::string & systemName, Context *);
                IEventLoop();

            public:
                virtual ~IEventLoop() = 0;

                void setRootObject(const std::shared_ptr<IObject> &);

                const std::shared_ptr<IObject> & getHover() const;
                void setHover(const std::shared_ptr<IObject> &);

            protected:
                const std::shared_ptr<IObject> & _getRootObject() const;

                void _pointerMove(const Event::PointerInfo&);
                void _buttonPress(int);
                void _buttonRelease(int);
                void _keyPress(int key, int mods);
                void _keyRelease(int key, int mods);
                void _drop(const std::vector<std::string> &);

                virtual void _hover(Event::PointerMove &, std::shared_ptr<IObject> &) = 0;

                void _tick(float dt);

            private:
                void _getFirstTick(const std::shared_ptr<IObject> &, std::vector<std::shared_ptr<IObject> > &);
                void _localeChangedRecursive(const std::shared_ptr<IObject> &, LocaleChanged &);
                void _updateRecursive(const std::shared_ptr<IObject> &, Update &);

                DJV_PRIVATE();

                friend class Context;
            };

        } // namespace Event
    } // namespace Core
} // namespace djv