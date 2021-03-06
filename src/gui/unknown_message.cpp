/*
 * Copyright (C) 2013  Maxim Noah Khailo
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtWidgets>

#include "gui/unknown_message.hpp"
#include "util/dbc.hpp"

namespace fire
{
    namespace gui
    {
        unknown_message::unknown_message(const std::string& t)
        {
            INVARIANT(root());
            INVARIANT(layout());

            _text = new QLabel{t.c_str()};
            layout()->addWidget(_text);

            INVARIANT(_text);
        }
    }
}
