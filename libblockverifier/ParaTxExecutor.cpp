/*
 * @CopyRight:
 * FISCO-BCOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FISCO-BCOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FISCO-BCOS.  If not, see <http://www.gnu.org/licenses/>
 * (c) 2016-2018 fisco-dev contributors.
 */

/**
 * @brief : implementation of parallel transactions executor
 * @file: ParaTxExecutor.cpp
 * @author: catli
 * @date: 2018-01-09
 */
#include <libblockverifier/ParaTxExecutor.h>
#include <cassert>
#include <mutex>

using namespace dev;
using namespace dev::blockverifier;
using namespace std;

void ParaTxWorker::doWork()
{
    if (m_txDAG == nullptr)
    {
        return;
    }

    assert(m_txDAG != nullptr && m_countDownLatch != nullptr);

    while (!m_txDAG->hasFinished())
    {
        m_txDAG->executeUnit();
    }
    m_txDAG.reset();
    m_countDownLatch->countDown();
}

void ParaTxWorker::workLoop()
{
    while (workerState() == WorkerState::Started)
    {
        m_wakeupNotifier->wait();
        doWork();
    }
}

void ParaTxExecutor::initialize(unsigned _threadNum)
{
    m_workers.reserve(_threadNum);
    for (auto i = _threadNum; i > 0; --i)
    {
        m_workers.push_back(ParaTxWorker(m_wakeupNotifier));
        m_workers.back().start();
    }
}

void ParaTxExecutor::start(shared_ptr<TxDAG> _txDAG)
{
    auto countDownLatch = make_shared<CountDownLatch>(threadNum());

    for (auto& worker : m_workers)
    {
        worker.setDAG(_txDAG);
        worker.setCountDownLatch(countDownLatch);
    }

    m_wakeupNotifier->notify();
    countDownLatch->wait();
    m_wakeupNotifier->reset();
}
