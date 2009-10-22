/*
 * This file is part of FBIde project
 *
 * FBIde is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FBIde is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FBIde.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Albert Varaksin <albeva@me.com>
 * Copyright (C) The FBIde development team
 */

#pragma once

namespace fb
{

    /**
     * Manage open documents. The type or content is
     * unimportant. Each document has various states
     * such as is it saved, can undo / redo be performed
     * and so on. Each document resides in the main
     * document tab area (or floating?)
     */
    struct SDK_DLL DocManager : private NonCopyable
    {

        // declare this class as a manager
        DECLARE_MANAGER(DocManager)

    };

}
