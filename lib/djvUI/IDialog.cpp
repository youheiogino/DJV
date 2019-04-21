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

#include <djvUI/IDialog.h>

#include <djvUI/Border.h>
#include <djvUI/Label.h>
#include <djvUI/Overlay.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ToolButton.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace
        {
            class DialogLayout : public VerticalLayout
            {
                DJV_NON_COPYABLE(DialogLayout);

            protected:
                void _init(Context * context)
                {
                    Vertical::_init(context);
                    setPointerEnabled(true);
                }
                
                DialogLayout()
                {}

            public:
                static std::shared_ptr<DialogLayout> create(Context * context)
                {
                    auto out = std::shared_ptr<DialogLayout>(new DialogLayout);
                    out->_init(context);
                    return out;
                }

            protected:
                void _buttonPressEvent(Event::ButtonPress & event) override
                {
                    event.accept();
                }

                void _buttonReleaseEvent(Event::ButtonRelease & event) override
                {
                    event.accept();
                }
            };

        } // namespace

        struct IDialog::Private
        {
            std::shared_ptr<Label> titleLabel;
            std::shared_ptr<ToolButton> closeButton;
            std::shared_ptr<VerticalLayout> childLayout;
            std::shared_ptr<VerticalLayout> layout;
            std::shared_ptr<Layout::Border> border;
            std::shared_ptr<Layout::Overlay> overlay;
            std::function<void(void)> closeCallback;
        };

        void IDialog::_init(Context * context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setPointerEnabled(true);

            p.titleLabel = Label::create(context);
            p.titleLabel->setFontSizeRole(MetricsRole::FontHeader);
            p.titleLabel->setTextHAlign(TextHAlign::Left);
            p.titleLabel->setMargin(MetricsRole::Margin);
            p.titleLabel->hide();

            p.closeButton = ToolButton::create(context);
            p.closeButton->setIcon("djvIconClose");
            p.closeButton->setInsideMargin(MetricsRole::MarginSmall);

            p.childLayout = VerticalLayout::create(context);
            p.childLayout->setSpacing(MetricsRole::None);

            p.layout = DialogLayout::create(context);
            p.layout->setSpacing(MetricsRole::None);
            p.layout->setBackgroundRole(ColorRole::Background);
            auto hLayout = HorizontalLayout::create(context);
            hLayout->setBackgroundRole(ColorRole::Header);
            hLayout->addChild(p.titleLabel);
            hLayout->addExpander();
            hLayout->addChild(p.closeButton);
            p.layout->addChild(hLayout);
            p.layout->addSeparator();
            p.layout->addChild(p.childLayout);
            p.layout->setStretch(p.childLayout, RowStretch::Expand);

            p.border = Layout::Border::create(context);
            p.border->setMargin(MetricsRole::MarginDialog);
            p.border->addChild(p.layout);

            p.overlay = Layout::Overlay::create(context);
            p.overlay->setCaptureKeyboard(false);
            p.overlay->addChild(p.border);
            Widget::addChild(p.overlay);

            auto weak = std::weak_ptr<IDialog>(std::dynamic_pointer_cast<IDialog>(shared_from_this()));
            p.closeButton->setClickedCallback(
                [weak]
            {
                if (auto dialog = weak.lock())
                {
                    dialog->_doCloseCallback();
                    dialog->hide();
                }
            });
            p.overlay->setCloseCallback(
                [weak]
            {
                if (auto dialog = weak.lock())
                {
                    dialog->_doCloseCallback();
                    dialog->hide();
                }
            });
        }

        IDialog::IDialog() :
            _p(new Private)
        {}

        IDialog::~IDialog()
        {}

        void IDialog::setTitle(const std::string & text)
        {
            DJV_PRIVATE_PTR();
            p.titleLabel->setText(text);
            p.titleLabel->setVisible(!text.empty());
        }

        void IDialog::setStretch(const std::shared_ptr<Widget> & widget, RowStretch value)
        {
            _p->childLayout->setStretch(widget, value);
        }

        void IDialog::setFillLayout(bool value)
        {
            DJV_PRIVATE_PTR();
            p.border->setHAlign(value ? HAlign::Fill : HAlign::Center);
            p.border->setVAlign(value ? VAlign::Fill : VAlign::Center);
        }

        void IDialog::setCloseCallback(const std::function<void(void)> & value)
        {
            _p->closeCallback = value;
        }

        void IDialog::setVisible(bool value)
        {
            Widget::setVisible(value);
            DJV_PRIVATE_PTR();
            p.overlay->moveToFront();
            p.overlay->setVisible(value);
        }

        float IDialog::getHeightForWidth(float value) const
        {
            return _p->overlay->getHeightForWidth(value);
        }

        void IDialog::addChild(const std::shared_ptr<IObject> & value)
        {
            _p->childLayout->addChild(value);
        }

        void IDialog::removeChild(const std::shared_ptr<IObject> & value)
        {
            _p->childLayout->removeChild(value);
        }

        void IDialog::_doCloseCallback()
        {
            if (_p->closeCallback)
            {
                _p->closeCallback();
            }
        }

        void IDialog::_preLayoutEvent(Event::PreLayout &)
        {
            _setMinimumSize(_p->overlay->getMinimumSize());
        }

        void IDialog::_layoutEvent(Event::Layout &)
        {
            _p->overlay->setGeometry(getGeometry());
        }

        void IDialog::_pointerEnterEvent(Core::Event::PointerEnter & event)
        {
            event.accept();
        }

        void IDialog::_pointerLeaveEvent(Core::Event::PointerLeave & event)
        {
            event.accept();
        }

        void IDialog::_pointerMoveEvent(Core::Event::PointerMove & event)
        {
            event.accept();
        }

        void IDialog::_keyPressEvent(Event::KeyPress & event)
        {
            Widget::_keyPressEvent(event);
            event.accept();
        }

        void IDialog::_keyReleaseEvent(Event::KeyRelease & event)
        {
            event.accept();
        }

    } // namespace UI
} // namespace djv