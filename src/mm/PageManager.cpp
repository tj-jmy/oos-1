#include "PageManager.h"
#include "Allocator.h"

unsigned int PageManager::PHY_MEM_SIZE;
unsigned int UserPageManager::USER_PAGE_POOL_SIZE;

PageManager::PageManager(Allocator *allocator)
{
	this->m_pAllocator = allocator;
}

int PageManager::Initialize()
{
	// for (unsigned int i = 0; i < MEMORY_MAP_ARRAY_SIZE; i++)
	// {
	// 	this->map[i].m_AddressIdx = 0;
	// 	this->map[i].m_Size = 0;
	// }
	for (unsigned int i = 0; i < TOTAL_PAGES / 8; i++)
	{
		this->m_PageBitmap[i] = 0;
	}
	for (unsigned int i = 0; i < TOTAL_PAGES; i++)
	{
		this->m_PageRefCnt[i] = 0;
	}
	return 0;
}

unsigned long PageManager::AllocMemory(unsigned long size)
{
	if (size == 0)
	{
		return 0;
	}

	unsigned long required_pages = (size + PAGE_SIZE - 1) / PAGE_SIZE;

	unsigned long startPageIndex = 0; // 起始页索引
	unsigned long count = 0;		  // 连续空闲页计数

	for (unsigned long i = 0; i < TOTAL_PAGES; ++i)
	{
		if (!bit_is_set(i))
		{
			if (count == 0)
				startPageIndex = i;
			count++;
			if (count == required_pages)
			{
				// 找到足够页，设置为占用
				for (unsigned long j = startPageIndex; j < startPageIndex + required_pages; ++j)
					set_bit(j);
				// 计算分配的物理地址
				return (m_StartAddressIdx + startPageIndex) * PAGE_SIZE;
			}
		}
		else
		{
			count = 0; // 当前页被占用，重置计数
		}
	}

	return 0;
}

unsigned long PageManager::FreeMemory(unsigned long size, unsigned long startAddress)
{
	// return this->m_pAllocator->Free(this->map,
	// 								(size + (PAGE_SIZE - 1)) / PAGE_SIZE, startAddress / PAGE_SIZE);
	unsigned long startIdx = startAddress / PageManager::PAGE_SIZE;
	unsigned long count = (size + (PageManager::PAGE_SIZE - 1)) / PageManager::PAGE_SIZE;

	for (unsigned long i = startIdx; i < startIdx + count; i++)
	{
		clear_bit(i);
	}

	return 0;
}

PageManager::~PageManager()
{
}

bool PageManager::bit_is_set(unsigned long index) const
{
	return this->m_PageBitmap[index / 8] & (1 << (index % 8));
}

void PageManager::set_bit(unsigned long index)
{
	this->m_PageBitmap[index / 8] |= (1 << (index % 8));
}

void PageManager::clear_bit(unsigned long index)
{
	this->m_PageBitmap[index / 8] &= ~(1 << (index % 8));
}

KernelPageManager::KernelPageManager(Allocator *allocator)
	: PageManager(allocator)
{
}

int KernelPageManager::Initialize()
{
	PageManager::Initialize();

	// this->map[0].m_AddressIdx =
	// 	KERNEL_PAGE_POOL_START_ADDR / PageManager::PAGE_SIZE;
	// this->map[0].m_Size =
	// 	KERNEL_PAGE_POOL_SIZE / PageManager::PAGE_SIZE;
	m_StartAddressIdx = KERNEL_PAGE_POOL_START_ADDR / PageManager::PAGE_SIZE;
	m_Size = KERNEL_PAGE_POOL_SIZE / PageManager::PAGE_SIZE;
	return 0;
}

UserPageManager::UserPageManager(Allocator *allocator)
	: PageManager(allocator)
{
}

int UserPageManager::Initialize()
{
	PageManager::Initialize();

	// this->map[0].m_AddressIdx =
	// 	USER_PAGE_POOL_START_ADDR / PageManager::PAGE_SIZE;
	// this->map[0].m_Size =
	// 	USER_PAGE_POOL_SIZE / PageManager::PAGE_SIZE;
	m_StartAddressIdx = USER_PAGE_POOL_START_ADDR / PageManager::PAGE_SIZE;
	m_Size = USER_PAGE_POOL_SIZE / PageManager::PAGE_SIZE;
	return 0;
}
