#include "XMemPool.h"

namespace XSpace_Common
{
	XMemPool::XMemPool()
	{
		m_xBlockInfo.clear();
	}

	XMemPool::~XMemPool()
	{
		ClearAllMem(); 
	}

	int XMemPool::InitMemPool( XMemPoolCfg& _xMemPoolCfg /*= XMemPoolCfg()*/)
	{
		m_xLock.lock();
		for (list<XMemBlockCfg>::iterator itr = _xMemPoolCfg.blockCfgList.begin(); itr != _xMemPoolCfg.blockCfgList.end(); itr ++)
		{
			XMemBlock* pBlock = new XMemBlock(itr->unBlockSize, itr->unSlgObjSize);
			m_xBlockInfo.push_back(XBlockInfo(itr->unSlgObjSize, pBlock));
			pBlock = NULL;
		}
		m_xLock.unlock();
		return 0;
	}

	void* XMemPool::AllocMem(const quint64& _unSize)
	{
		// Find Block
		XMemBlock* pBlock = NULL;
		m_xLock.lock();
		for (list<XBlockInfo>::iterator itr = m_xBlockInfo.begin(); itr != m_xBlockInfo.end(); itr++)
		{
			if (itr->unSlgObjSize >= _unSize && itr->unSlgObjSize - _unSize < 8)
			{
				pBlock = itr->pxBlockHead;
				break;
			}
		}
		if (pBlock == NULL)
		{
			pBlock = new XMemBlock(_unSize*256, _unSize);
			m_xBlockInfo.push_back(XBlockInfo(_unSize, pBlock));
		}
		m_xLock.unlock();

		void* pRet =  pBlock->gmem();
		if (pRet == NULL)
		{
			// 无可用内存块
			XMemBlock* pNewBlock = new XMemBlock(pBlock->unSizeForBlock, pBlock->unSizeForSlgObj);
			pBlock->pNextBlock = pNewBlock;
			pRet = pNewBlock->gmem();
		}
		return pRet;
	}

	void XMemPool::FreeMem(void* _pMem, const quint64& _unSize)
	{
		// Find Block
		XMemBlock* pBlock = NULL;
		m_xLock.lock();
		for (list<XBlockInfo>::iterator itr = m_xBlockInfo.begin(); itr != m_xBlockInfo.end(); itr++)
		{
			if (itr->unSlgObjSize >= _unSize && itr->unSlgObjSize - _unSize < 8)
			{
				pBlock = itr->pxBlockHead;
				break;
			}
		}
		m_xLock.unlock();
		if (pBlock)
		{
			pBlock->fmem(_pMem);
		}
	}

	void XMemPool::ClearAllMem()
	{
		m_xLock.lock();
		for (list<XBlockInfo>::iterator itr = m_xBlockInfo.begin(); itr != m_xBlockInfo.end(); itr++)
		{
			itr->destory();
		}
		m_xBlockInfo.clear();
		m_xLock.unlock();
	}

}