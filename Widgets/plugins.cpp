/****************************************************************************
** Copyright (C) 2017 Olaf Japp
**
** This file is part of AnimationMaker.
**
**  AnimationMaker is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  AnimationMaker is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with AnimationMaker.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "plugins.h"

QMap<QString, ItemInterface*> Plugins::itemPlugins;

ItemInterface *Plugins::getItemPlugin(QString name)
{
    return itemPlugins[name];
}

bool Plugins::hasItemPlugin(QString name)
{
    return itemPlugins.contains(name);
}

QList<QString> Plugins::itemPluginNames()
{
    return itemPlugins.keys();
}

void Plugins::insert(QString name, ItemInterface *plugin)
{
    itemPlugins.insert(name, plugin);
}
