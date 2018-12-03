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

#include <sstream>

namespace djv
{
    template<>
    inline picojson::value toJSON<UICore::FontMap>(const UICore::FontMap& value)
    {
        picojson::value out(picojson::object_type, true);
        for (const auto& i : value)
        {
            std::stringstream ss;
            ss << i.first;
            out.get<picojson::object>()[ss.str()] = picojson::value(i.second);
        }
        return out;
    }

    template<>
    inline picojson::value toJSON<std::string, UICore::FontMap>(const std::map<std::string, UICore::FontMap>& value)
    {
        picojson::value out(picojson::object_type, true);
        for (const auto& i : value)
        {
            out.get<picojson::object>()[i.first] = toJSON(i.second);
        }
        return out;
    }

    template<>
    inline void fromJSON<UICore::FontMap>(const picojson::value& value, UICore::FontMap& out)
    {
        if (value.is<picojson::object>())
        {
            for (const auto& i : value.get<picojson::object>())
            {
                UICore::FontFace v = UICore::FontFace::First;
                std::stringstream ss(i.first);
                ss >> v;
                std::string s;
                fromJSON(i.second, s);
                out[v] = s;
            }
        }
        else
        {
            throw std::invalid_argument("Cannot parse");
        }
    }

    template<>
    inline void fromJSON<std::string, UICore::FontMap>(const picojson::value& value, std::map<std::string, UICore::FontMap>& out)
    {
        if (value.is<picojson::object>())
        {
            for (const auto& i : value.get<picojson::object>())
            {
                fromJSON(i.second, out[i.first]);
            }
        }
        else
        {
            throw std::invalid_argument("Cannot parse");
        }
    }

} // namespace djv

