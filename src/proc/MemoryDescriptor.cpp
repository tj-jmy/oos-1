#include "MemoryDescriptor.h"
#include "Kernel.h"
#include "PageManager.h"
#include "Machine.h"
#include "PageDirectory.h"
#include "Video.h"

void MemoryDescriptor::Initialize()
{
	KernelPageManager &kernelPageManager = Kernel::Instance().GetKernelPageManager();

	/* m_UserPageTableArray需要把AllocMemory()返回的物理内存地址 + 0xC0000000 */
	// this->m_UserPageTableArray = (PageTable *)(kernelPageManager.AllocMemory(sizeof(PageTable) * USER_SPACE_PAGE_TABLE_CNT) + Machine::KERNEL_SPACE_START_ADDRESS);
	this->m_UserPageTableArray = NULL;
}

void MemoryDescriptor::Release()
{
	KernelPageManager &kernelPageManager = Kernel::Instance().GetKernelPageManager();
	if (this->m_UserPageTableArray)
	{
		kernelPageManager.FreeMemory(sizeof(PageTable) * USER_SPACE_PAGE_TABLE_CNT, (unsigned long)this->m_UserPageTableArray - Machine::KERNEL_SPACE_START_ADDRESS);
		this->m_UserPageTableArray = NULL;
	}
}

unsigned int MemoryDescriptor::MapEntry(unsigned long virtualAddress, unsigned int size, unsigned long phyPageIdx, bool isReadWrite)
{
	// unsigned long address = virtualAddress - USER_SPACE_START_ADDRESS;

	// // 计算从pagetable的哪一个地址开始映射
	// unsigned long startIdx = address >> 12;
	// unsigned long cnt = (size + (PageManager::PAGE_SIZE - 1)) / PageManager::PAGE_SIZE;

	// PageTableEntry *entrys = (PageTableEntry *)this->m_UserPageTableArray;
	// for (unsigned int i = startIdx; i < startIdx + cnt; i++, phyPageIdx++)
	// {
	// 	entrys[i].m_Present = 0x1;
	// 	entrys[i].m_ReadWriter = isReadWrite;
	// 	entrys[i].m_PageBaseAddress = phyPageIdx;
	// }
	return phyPageIdx;
}

void MemoryDescriptor::MapTextEntrys(unsigned long textStartAddress, unsigned long textSize, unsigned long textPageIdx)
{
	this->MapEntry(textStartAddress, textSize, textPageIdx, false);
}
void MemoryDescriptor::MapDataEntrys(unsigned long dataStartAddress, unsigned long dataSize, unsigned long dataPageIdx)
{
	this->MapEntry(dataStartAddress, dataSize, dataPageIdx, true);
}

void MemoryDescriptor::MapStackEntrys(unsigned long stackSize, unsigned long stackPageIdx)
{
	unsigned long stackStartAddress = (USER_SPACE_START_ADDRESS + USER_SPACE_SIZE - stackSize) & 0xFFFFF000;
	this->MapEntry(stackStartAddress, stackSize, stackPageIdx, true);
}

PageTable *MemoryDescriptor::GetUserPageTableArray()
{
	// return this->m_UserPageTableArray;
	return NULL;
}
unsigned long MemoryDescriptor::GetTextStartAddress()
{
	return this->m_TextStartAddress;
}
unsigned long MemoryDescriptor::GetTextSize()
{
	return this->m_TextSize;
}
unsigned long MemoryDescriptor::GetDataStartAddress()
{
	return this->m_DataStartAddress;
}
unsigned long MemoryDescriptor::GetDataSize()
{
	return this->m_DataSize;
}
unsigned long MemoryDescriptor::GetStackSize()
{
	return this->m_StackSize;
}

bool MemoryDescriptor::EstablishUserPageTable(unsigned long textVirtualAddress, unsigned long textSize, unsigned long dataVirtualAddress, unsigned long dataSize, unsigned long stackSize)
{
	User &u = Kernel::Instance().GetUser();

	/* 如果超出允许的用户程序最大8M的地址空间限制 */
	if (textSize + dataSize + stackSize + PageManager::PAGE_SIZE > USER_SPACE_SIZE - textVirtualAddress)
	{
		u.u_error = User::ENOMEM;
		Diagnose::Write("u.u_error = %d\n", u.u_error);
		return false;
	}
	m_TextSize = textSize;
	m_DataSize = dataSize;
	m_StackSize = stackSize;
	// this->ClearUserPageTable();

	// /* 以相对起始地址phyPageIndex为0，为正文段建立相对地址映照表 */
	// unsigned int phyPageIndex = 0;
	// phyPageIndex = this->MapEntry(textVirtualAddress, textSize, phyPageIndex, false);

	// /* 以相对起始地址phyPageIndex为1，ppda区占用1页4K大小物理内存，为数据段建立相对地址映照表 */
	// phyPageIndex = 1;
	// phyPageIndex = this->MapEntry(dataVirtualAddress, dataSize, phyPageIndex, true);

	// /* 紧跟着数据段之后，为堆栈段建立相对地址映照表 */
	// unsigned long stackStartAddress = (USER_SPACE_START_ADDRESS + USER_SPACE_SIZE - stackSize) & 0xFFFFF000;
	// this->MapEntry(stackStartAddress, stackSize, phyPageIndex, true);

	// /* 将相对地址映照表根据正文段和数据段在内存中的起始地址pText->x_caddr、p_addr，建立用户态内存区的页表映射 */
	this->MapToPageTable();
	return true;
}

void MemoryDescriptor::ClearUserPageTable()
{
	// User &u = Kernel::Instance().GetUser();
	// PageTable *pUserPageTable = u.u_MemoryDescriptor.m_UserPageTableArray;

	// unsigned int i;
	// unsigned int j;

	// for (i = 0; i < Machine::USER_PAGE_TABLE_CNT; i++)
	// {
	// 	for (j = 0; j < PageTable::ENTRY_CNT_PER_PAGETABLE; j++)
	// 	{
	// 		pUserPageTable[i].m_Entrys[j].m_Present = 0;
	// 		pUserPageTable[i].m_Entrys[j].m_ReadWriter = 0;
	// 		pUserPageTable[i].m_Entrys[j].m_UserSupervisor = 1;
	// 		pUserPageTable[i].m_Entrys[j].m_PageBaseAddress = 0;
	// 	}
	// }
}

void MemoryDescriptor::MapToPageTable()
{
	User &u = Kernel::Instance().GetUser(); // 获取当前用户结构的引用

	// if (u.u_MemoryDescriptor.m_UserPageTableArray == NULL) // 如果用户页表数组为空，则直接返回
	// 	return;

	PageTable *pUserPageTable = Machine::Instance().GetUserPageTableArray(); // 获取内核空间的用户页表数组
	unsigned int textAddress = 0;											 // 文本段的起始地址
	// 如果进程的文本段存在，则获取其起始地址
	if (u.u_procp->p_textp != NULL)
	{
		textAddress = u.u_procp->p_textp->x_caddr;
	}

	unsigned int textstartindex = 0, datastartindex = 1;

	unsigned int textcnt = (m_TextSize + (PageManager::PAGE_SIZE - 1)) / PageManager::PAGE_SIZE;
	unsigned int datacnt = (m_DataSize + (PageManager::PAGE_SIZE - 1)) / PageManager::PAGE_SIZE;
	unsigned int stackcnt = (m_StackSize + (PageManager::PAGE_SIZE - 1)) / PageManager::PAGE_SIZE;

	unsigned int dataindex = 0;

	// 遍历用户页表数组中的每个页表
	for (unsigned int i = 0; i < Machine::USER_PAGE_TABLE_CNT; i++)
	{
		// 遍历每个页表中的每个条目
		for (unsigned int j = 0; j < PageTable::ENTRY_CNT_PER_PAGETABLE; j++)
		{
			pUserPageTable[i].m_Entrys[j].m_Present = 0; // 先清0，标记为不在内存中
			// 刷新第二个用户页表
			if (1 == i)
			{
				// 1到textcnt表示文本段对应的页，以textAddress为内存起始地址
				if (j >= 1 && j <= textcnt)
				{
					pUserPageTable[i].m_Entrys[j].m_Present = 1;
					pUserPageTable[i].m_Entrys[j].m_ReadWriter = 0;
					pUserPageTable[i].m_Entrys[j].m_PageBaseAddress = (textAddress >> 12) + j - 1 + textstartindex;
				}
				// textcnt到textcnt+datacnt表示数据段对应的页，以p_addr为内存起始地址
				else if (j > textcnt && j <= textcnt + datacnt)
				{
					pUserPageTable[i].m_Entrys[j].m_Present = 1;
					pUserPageTable[i].m_Entrys[j].m_ReadWriter = 1;
					pUserPageTable[i].m_Entrys[j].m_PageBaseAddress = (u.u_procp->p_addr >> 12) + dataindex + datastartindex;
					dataindex++;
				}
				// 堆栈段对应的页，以p_addr为内存起始地址
				else if (j >= PageTable::ENTRY_CNT_PER_PAGETABLE - stackcnt)
				{
					pUserPageTable[i].m_Entrys[j].m_Present = 1;
					pUserPageTable[i].m_Entrys[j].m_ReadWriter = 1;
					pUserPageTable[i].m_Entrys[j].m_PageBaseAddress = (u.u_procp->p_addr >> 12) + dataindex + datastartindex;
					dataindex++;
				}
			}
		}
	}
	// 设置用户页表数组的第一个页表的第一个条目
	pUserPageTable[0].m_Entrys[0].m_Present = 1;
	pUserPageTable[0].m_Entrys[0].m_ReadWriter = 1;
	pUserPageTable[0].m_Entrys[0].m_PageBaseAddress = 0;

	FlushPageDirectory();
}
