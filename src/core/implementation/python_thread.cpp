/*
 * Utils responsible for managing Python's thread.
 * Copyright (C) 2018  Michał Walenciak <Kicer86@gmail.com>
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

#include "python_thread.hpp"

#include <pybind11/embed.h>

#include <system/filesystem.hpp>


namespace
{
    void init_python()
    {
        const QString scripts_qstr = FileSystem().getScriptsPath();
        const std::string scripts_str = scripts_qstr.toStdString();

        pybind11::module sys = pybind11::module::import("sys");
        sys.attr("path").cast<pybind11::list>().append(scripts_str);
        pybind11::print(sys.attr("path"));
    }

    void deinit_python()
    {
        // Nothing special to do
    }
}


struct PythonThread::Impl
{
    pybind11::scoped_interpreter m_py_interpreter;
    ol::TS_Queue<std::function<void ()>> m_tasks;

    Impl(): m_py_interpreter(), m_tasks(16) {}
};


PythonThread::PythonThread():m_impl(new Impl), m_pythonThread(std::bind(&PythonThread::thread, this))
{
    execute(&init_python);
}


PythonThread::~PythonThread()
{
    execute(&deinit_python);
    m_impl->m_tasks.stop();
    m_pythonThread.join();
}


void PythonThread::execute(const std::function<void ()>& callback)
{
    m_impl->m_tasks.push(callback);
}


void PythonThread::thread()
{
    for(;;)
    {
        auto task = m_impl->m_tasks.pop();

        if (task)
        {
            const auto& callable = *task;
            callable();
        }
        else
            break;
    }
}

