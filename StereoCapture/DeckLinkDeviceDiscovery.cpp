/* -LICENSE-START-
** Copyright (c) 2020 Blackmagic Design
**  
** Permission is hereby granted, free of charge, to any person or organization 
** obtaining a copy of the software and accompanying documentation (the 
** "Software") to use, reproduce, display, distribute, sub-license, execute, 
** and transmit the Software, and to prepare derivative works of the Software, 
** and to permit third-parties to whom the Software is furnished to do so, in 
** accordance with:
** 
** (1) if the Software is obtained from Blackmagic Design, the End User License 
** Agreement for the Software Development Kit (“EULA”) available at 
** https://www.blackmagicdesign.com/EULA/DeckLinkSDK; or
** 
** (2) if the Software is obtained from any third party, such licensing terms 
** as notified by that third party,
** 
** and all subject to the following:
** 
** (3) the copyright notices in the Software and this entire statement, 
** including the above license grant, this restriction and the following 
** disclaimer, must be included in all copies of the Software, in whole or in 
** part, and all derivative works of the Software, unless such copies or 
** derivative works are solely in the form of machine-executable object code 
** generated by a source language processor.
** 
** (4) THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
** OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
** FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT 
** SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE 
** FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE, 
** ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
** DEALINGS IN THE SOFTWARE.
** 
** A copy of the Software is available free of charge at 
** https://www.blackmagicdesign.com/desktopvideo_sdk under the EULA.
** 
** -LICENSE-END-
*/

#include "pch.h"
#include <stdexcept>
#include "DeckLinkDeviceDiscovery.h"

DeckLinkDeviceDiscovery::DeckLinkDeviceDiscovery() : 
	m_deckLinkDiscovery(nullptr), 
	m_refCount(1)
{
	if (CoCreateInstance(CLSID_CDeckLinkDiscovery, NULL, CLSCTX_ALL, IID_IDeckLinkDiscovery, (void**)&m_deckLinkDiscovery) != S_OK)
		throw std::runtime_error("Unable to create IDeckLinkDiscovery interface object");
}

bool DeckLinkDeviceDiscovery::enable()
{
	HRESULT result = E_FAIL;

	// Install device arrival notifications
	if (m_deckLinkDiscovery)
		result = m_deckLinkDiscovery->InstallDeviceNotifications(this);

	return result == S_OK;
}

void DeckLinkDeviceDiscovery::disable()
{
	// Uninstall device arrival notifications
	if (m_deckLinkDiscovery)
		m_deckLinkDiscovery->UninstallDeviceNotifications();
}

HRESULT DeckLinkDeviceDiscovery::DeckLinkDeviceArrived(/* in */ IDeckLink* deckLink)
{
	if (m_deckLinkArrivedCallback)
	{
		CComPtr<IDeckLink> deckLinkDevice = deckLink;
		m_deckLinkArrivedCallback(deckLinkDevice);
	}

	return S_OK;
}

HRESULT DeckLinkDeviceDiscovery::DeckLinkDeviceRemoved(/* in */ IDeckLink* deckLink)
{
	if (m_deckLinkRemovedCallback)
	{
		CComPtr<IDeckLink> deckLinkDevice = deckLink;
		m_deckLinkRemovedCallback(deckLinkDevice);
	}

	return S_OK;
}

HRESULT DeckLinkDeviceDiscovery::QueryInterface(REFIID iid, LPVOID *ppv)
{
	HRESULT result = E_NOINTERFACE;

	if (ppv == NULL)
		return E_INVALIDARG;

	// Initialise the return result
	*ppv = NULL;

	// Obtain the IUnknown interface and compare it the provided REFIID
	if (iid == IID_IUnknown)
	{
		*ppv = this;
		AddRef();
		result = S_OK;
	}
	else if (iid == IID_IDeckLinkDeviceNotificationCallback)
	{
		*ppv = static_cast<IDeckLinkDeviceNotificationCallback*>(this);
		AddRef();
		result = S_OK;
	}

	return result;
}

ULONG DeckLinkDeviceDiscovery::AddRef(void)
{
	return ++m_refCount;
}

ULONG DeckLinkDeviceDiscovery::Release(void)
{
	ULONG newRefValue = --m_refCount;
	if (newRefValue == 0)
		delete this;

	return newRefValue;
}
