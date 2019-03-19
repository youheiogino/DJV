//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvUI/FloatEdit.h>

#include <djvUI/Action.h>
#include <djvUI/LineEdit.h>

#include <djvCore/NumericValueModels.h>
#include <djvCore/ValueObserver.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct FloatEdit::Private
        {
            std::shared_ptr<FloatValueModel> model;
            int precision = 2;
            std::shared_ptr<LineEdit> lineEdit;
            std::shared_ptr<ValueObserver<FloatRange> > rangeObserver;
            std::shared_ptr<ValueObserver<float> > valueObserver;
            std::shared_ptr<ValueObserver<bool> > incrementSmallObserver;
            std::shared_ptr<ValueObserver<bool> > incrementLargeObserver;
            std::shared_ptr<ValueObserver<bool> > decrementSmallObserver;
            std::shared_ptr<ValueObserver<bool> > decrementLargeObserver;
        };

        void FloatEdit::_init(Context * context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();

            setClassName("djv::UI::FloatEdit");

            auto incrementSmallAction = Action::create();
            incrementSmallAction->setShortcut(GLFW_KEY_UP);
            addAction(incrementSmallAction);
            auto incrementLargeAction = Action::create();
            incrementLargeAction->setShortcut(GLFW_KEY_PAGE_UP);
            addAction(incrementLargeAction);
            auto decrementSmallAction = Action::create();
            decrementSmallAction->setShortcut(GLFW_KEY_DOWN);
            addAction(decrementSmallAction);
            auto decrementLargeAction = Action::create();
            decrementLargeAction->setShortcut(GLFW_KEY_PAGE_DOWN);
            addAction(decrementLargeAction);

            p.lineEdit = LineEdit::create(context);
            p.lineEdit->setFont("Noto Sans Mono");
            addChild(p.lineEdit);

            _textUpdate();;

            auto weak = std::weak_ptr<FloatEdit>(std::dynamic_pointer_cast<FloatEdit>(shared_from_this()));
            p.lineEdit->setTextFinishedCallback(
                [weak](const std::string & value)
            {
                if (auto widget = weak.lock())
                {
                    if (widget->_p->model)
                    {
                        try
                        {
                            widget->_p->model->setValue(std::stof(value));
                        }
                        catch (const std::exception &)
                        {
                        }
                        widget->_textUpdate();
                    }
                }
            });

            p.incrementSmallObserver = ValueObserver<bool>::create(
                incrementSmallAction->observeClicked(),
                [weak](bool value)
            {
                if (value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->model->incrementSmall();
                    }
                }
            });

            p.incrementLargeObserver = ValueObserver<bool>::create(
                incrementLargeAction->observeClicked(),
                [weak](bool value)
            {
                if (value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->model->incrementLarge();
                    }
                }
            });

            p.decrementSmallObserver = ValueObserver<bool>::create(
                decrementSmallAction->observeClicked(),
                [weak](bool value)
            {
                if (value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->model->decrementSmall();
                    }
                }
            });

            p.decrementLargeObserver = ValueObserver<bool>::create(
                decrementLargeAction->observeClicked(),
                [weak](bool value)
            {
                if (value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->model->decrementLarge();
                    }
                }
            });
        }

        FloatEdit::FloatEdit() :
            _p(new Private)
        {}

        FloatEdit::~FloatEdit()
        {}

        std::shared_ptr<FloatEdit> FloatEdit::create(Context * context)
        {
            auto out = std::shared_ptr<FloatEdit>(new FloatEdit);
            out->_init(context);
            return out;
        }

        const std::shared_ptr<FloatValueModel> & FloatEdit::getModel() const
        {
            return _p->model;
        }

        void FloatEdit::setModel(const std::shared_ptr<FloatValueModel> & model)
        {
            DJV_PRIVATE_PTR();
            if (p.model)
            {
                p.valueObserver.reset();
            }
            p.model = model;
            if (p.model)
            {
                auto weak = std::weak_ptr<FloatEdit>(std::dynamic_pointer_cast<FloatEdit>(shared_from_this()));
                p.rangeObserver = ValueObserver<FloatRange>::create(
                    p.model->observeRange(),
                    [weak](const FloatRange & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_textUpdate();
                    }
                });
                p.valueObserver = ValueObserver<float>::create(
                    p.model->observeValue(),
                    [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_textUpdate();
                    }
                });
            }
        }

        int FloatEdit::getPrecision()
        {
            return _p->precision;
        }

        void FloatEdit::setPrecision(int value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.precision)
                return;
            p.precision = value;
            _textUpdate();
        }

        void FloatEdit::_preLayoutEvent(Event::PreLayout& event)
        {
            DJV_PRIVATE_PTR();
            if (auto style = _getStyle().lock())
            {
                _setMinimumSize(p.lineEdit->getMinimumSize() + getMargin().getSize(style));
            }
        }

        void FloatEdit::_layoutEvent(Event::Layout& event)
        {
            DJV_PRIVATE_PTR();
            if (auto style = _getStyle().lock())
            {
                p.lineEdit->setGeometry(getMargin().bbox(getGeometry(), style));
            }
        }

        void FloatEdit::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.model)
            {
                {
                    std::stringstream ss;
                    ss.precision(p.precision);
                    ss << std::fixed << p.model->observeValue()->get();
                    p.lineEdit->setText(ss.str());

                    const auto & range = p.model->observeRange()->get();
                    const size_t digits = std::max(Math::getNumDigits(range.min), Math::getNumDigits(range.max));
                    std::string sizeString;
                    if (range.min < 0 || range.max < 0)
                    {
                        sizeString += '-';
                    }
                    sizeString += std::string(digits, '0');
                    sizeString += '.';
                    for (size_t i = 0; i < p.precision; ++i)
                    {
                        sizeString += '0';
                    }
                    p.lineEdit->setSizeString(sizeString);
                }
            }
            _resize();
        }

    } // namespace UI
} // namespace djv
