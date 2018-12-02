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

#include <djvCore/TextSystem.h>

#include <djvCore/Context.h>
#include <djvCore/FileInfo.h>
#include <djvCore/FileIO.h>
#include <djvCore/OS.h>

#include <locale>
#include <future>
#include <set>

//#pragma optimize("", off)

namespace djv
{
    namespace Core
    {
        struct TextSystem::Private
        {
            std::vector<std::string> locales;
            std::string currentLocale;
            std::map<std::string, std::map<std::string, std::string> > text;

            std::thread thread;
            std::mutex mutex;
        };

        namespace
        {
            std::string parseLocale(const std::string& value)
            {
                std::string out;
#if defined(DJV_WINDOWS)
                //! \todo [1.0 M] Windows locale.
#elif defined(DJV_OSX)
                //! \todo [1.0 M] OS X locale.
#else
                auto pieces = String::split(value, '_');
                if (!pieces.size())
                {
                    pieces = String::split(value, '.');
                }
                if (pieces.size())
                {
                    out = pieces[0];
                }
#endif
                return out;
            }
        } // namespace

        void TextSystem::_init(const std::shared_ptr<Context>& context)
        {
            ISystem::_init("djv::Core::TextSystem", context);
            
            _p->thread = std::thread(
                [this, context]
            {
                std::unique_lock<std::mutex> lock(_p->mutex);

                _p->currentLocale = "en";
                std::string djvLocale = OS::getEnv("DJV_LANG");
                std::stringstream s;
                if (djvLocale.size())
                {
                    _p->currentLocale = djvLocale;
                }
                else
                {
                    try
                    {
                        std::locale locale("");
                        s << "Current std::locale: " << locale.name();
                        _log(s.str());
                        std::string cppLocale = parseLocale(locale.name());
                        if (cppLocale.size())
                        {
                            _p->currentLocale = cppLocale;
                        }
                    }
                    catch (const std::exception& e)
                    {
                        _log(e.what(), LogLevel::Error);
                    }
                }
                s.str(std::string());
                s << "Current locale: " << _p->currentLocale;
                _log(s.str());

                // Find the .text files.
                DirListOptions options;
                options.glob = "*.text";
                const std::vector<FileInfo> fileInfos = FileInfo::dirList(context->getResourcePath(ResourcePath::TextDirectory), options);

                // Extract the locale names.
                std::set<std::string> localeSet;
                for (const auto& fileInfo : fileInfos)
                {
                    std::string temp = Path(fileInfo.getPath().getBaseName()).getExtension();
                    if (temp.size() && '.' == temp[0])
                    {
                        temp.erase(temp.begin());
                    }
                    if (temp != "all")
                    {
                        localeSet.insert(temp);
                    }
                }
                for (const auto& locale : localeSet)
                {
                    _p->locales.push_back(locale);
                }
                s.str(std::string());
                s << "Found text files: " << String::join(_p->locales, ", ");
                _log(s.str());

                // Read the .text files.
                _readText();
            });
        }

        TextSystem::TextSystem() :
            _p(new Private)
        {}

        TextSystem::~TextSystem()
        {}

        std::shared_ptr<TextSystem> TextSystem::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<TextSystem>(new TextSystem);
            out->_init(context);
            return out;
        }

        const std::vector<std::string>& TextSystem::getLocales() const
        {
            std::unique_lock<std::mutex> lock(_p->mutex);
            return _p->locales;
        }

        const std::string& TextSystem::getCurrentLocale() const
        {
            std::unique_lock<std::mutex> lock(_p->mutex);
            return _p->currentLocale;
        }

        void TextSystem::setCurrentLocale(const std::string& value)
        {
            std::unique_lock<std::mutex> lock(_p->mutex);
            _p->currentLocale = value;
        }
        
        const std::string& TextSystem::getText(const std::string& id) const
        {
            std::unique_lock<std::mutex> lock(_p->mutex);
            const auto i = _p->text.find(_p->currentLocale);
            if (i != _p->text.end())
            {
                const auto j = i->second.find(id);
                if (j != i->second.end())
                {
                    return j->second;
                }
            }
            return id;
        }

        void TextSystem::_exit()
        {
            ISystem::_exit();
            _p->thread.join();
        }

        void TextSystem::_readText()
        {
            _p->text.clear();

            if (auto context = getContext().lock())
            {
                _log("Reading text files:");
                DirListOptions options;
                options.glob = "*.text";
                for (const auto& i : FileInfo::dirList(context->getResourcePath(ResourcePath::TextDirectory), options))
                {
                    _log(String::indent(1) + i.getPath().get());
                    try
                    {
                        const auto& path = i.getPath();
                        const auto& baseName = path.getBaseName();
                        std::string locale;
                        for (auto i = baseName.rbegin(); i != baseName.rend() && *i != '.'; ++i)
                        {
                            locale.insert(locale.begin(), *i);
                        }

                        FileIO fileIO;
                        fileIO.open(path, FileIO::Mode::Read);
                        const char* p = reinterpret_cast<const char*>(fileIO.mmapP());
                        const char* end = reinterpret_cast<const char*>(fileIO.mmapEnd());

                        // Parse the JSON.
                        picojson::value v;
                        std::string error;
                        picojson::parse(v, p, end, &error);
                        if (!error.empty())
                        {
                            std::stringstream s;
                            s << "Error reading text file: " << path << ": " << error;
                            throw std::runtime_error(s.str());
                        }

                        if (v.is<picojson::array>())
                        {
                            for (const auto& item : v.get<picojson::array>())
                            {
                                if (item.is<picojson::object>())
                                {
                                    std::string id;
                                    std::string text;
                                    std::string description;
                                    const auto& obj = item.get<picojson::object>();
                                    for (auto i = obj.begin(); i != obj.end(); ++i)
                                    {
                                        if ("id" == i->first)
                                        {
                                            id = i->second.to_str();
                                        }
                                        else if ("text" == i->first)
                                        {
                                            text = i->second.to_str();
                                        }
                                        else if ("description" == i->first)
                                        {
                                            description = i->second.to_str();
                                        }
                                    }
                                    if (!id.empty())
                                    {
                                        _p->text[locale][id] = text;
                                    }
                                }
                            }
                        }
                    }
                    catch (const std::exception& e)
                    {
                        _log(e.what(), LogLevel::Error);
                    }
                }
            }
        }

    } // namespace Core
} // namespace djv
