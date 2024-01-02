/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CTransferBox.cpp
 *  PURPOSE:     Transfer box GUI
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>

#define TRANSFERBOX_HEIGHT         58
#define TRANSFERBOX_ICONSIZE       20
#define TRANSFERBOX_PROGRESSHEIGHT 28
#define TRANSFERBOX_YSTART         20
#define TRANSFERBOX_SPACER         11

#define PROGRESS_BG_HEIGHT         20
#define PROGRESS_BG_WIDTH          750

CTransferBox::CTransferBox(TransferBoxType transferType) : m_GUI(g_pCore->GetGUI())
{
    switch (transferType)
    {
        case TransferBoxType::MAP_DOWNLOAD:
            m_titleProgressPrefix = _("Map download progress:");
            break;
        default:
            m_titleProgressPrefix = _("Download Progress:");
    }

    m_visible.set(TB_VISIBILITY_CLIENT_SCRIPT);
    m_visible.set(TB_VISIBILITY_SERVER_SCRIPT);

    g_pCore->GetCVars()->Get("always_show_transferbox", m_alwaysVisible);

    //CreateTransferWindow();
    CreateProgressBarBg();
}



void CTransferBox::CreateProgressBarBg()
{
    

    CVector2D screenSize = m_GUI->GetResolution();


    //pProgressBG->SetPosition(CVector2D(screenSize.fX * 0.7f - 80 * 0.5f, screenSize.fY * 0.70f - TRANSFERBOX_HEIGHT * 0.5f));
    pProgressBG->SetPosition(CVector2D(15.0f, screenSize.fY - 25.0f - 20.0f));

    //pProgressBG->SetSize( CVector2D(PROGRESS_BG_WIDTH, PROGRESS_BG_HEIGHT) );
    pProgressBG->SetSize( CVector2D ( screenSize.fX - 2 * 20.0f, PROGRESS_BG_HEIGHT ) );
    pProgressBG->LoadFromFile("cgui\\images\\progress_bar\\bg.png");
    pProgressBG->SetVisible(false);


    // MASK
    pProgressMask->SetPosition(CVector2D(0, 0));
    pProgressMask->LoadFromFile("cgui\\images\\progress_bar\\mask.png");
    pProgressMask->SetSize( CVector2D( 0, PROGRESS_BG_HEIGHT ) );
    pProgressMask->SetVisible(false);

    // DOWNLOAD PROGRESS LABEL

    pLabelDownloadProgress = reinterpret_cast<CGUILabel*>(m_GUI->CreateLabel("Завантаження ресурсів..."));
    pLabelDownloadProgress->SetSize( CVector2D(pLabelDownloadProgress->GetTextExtent() + 5, 20) );
    pLabelDownloadProgress->SetPosition( CVector2D(15, screenSize.fY - 25 * 2.70) );
    pLabelDownloadProgress->SetVisible(false);

    pLabelDownloadProgressF = reinterpret_cast<CGUILabel*>(m_GUI->CreateLabel(""));
    pLabelDownloadProgressF->SetSize(CVector2D(pLabelDownloadProgress->GetTextExtent() + 5, 20));
    //pLabelDownloadProgressF->SetPosition(CVector2D(screenSize.fX - ( pLabelDownloadProgressF->GetTextExtent() - 40.0f ), screenSize.fY - 25 * 2.70));
    pLabelDownloadProgressF->SetVisible(false);
}

void CTransferBox::UpdateProgressBarVisigle(bool state)
{
    pProgressBG->SetVisible(state);
}

void CTransferBox::SetProgressBarDownloadProgress(uint64_t downloadedSizeTotal)
{
    pProgressBG->SetVisible(true);
}

void CTransferBox::SetDownloadProgress(uint64_t downloadedSizeTotal)
{
    CVector2D screenSize = m_GUI->GetResolution();

    float MaskSize = (float)pProgressMask->GetSize().fX;
    // Кодировщик
    SString current = GetDataUnit(downloadedSizeTotal);
    SString total = GetDataUnit(m_downloadTotalSize);

    // Вычисление прогресса загрузки
    float progress = static_cast<float>(downloadedSizeTotal) / m_downloadTotalSize;
    //float progressBarWidth = 750.0f;            // Ширина вашего прогресс бара
    float progressBarWidth = screenSize.fX - 2 * 15.0f;
    float maskWidth = progressBarWidth * progress;

    // Записуємо значення в Label
    SString MbProgress_text = SString(_("%s / %s"), current.c_str(), total.c_str());
    // Міняємо візуально
    pProgressMask->SetSize( CVector2D( maskWidth, pProgressMask->GetSize().fY ) );

    pLabelDownloadProgress->SetText(m_titleProgressPrefix.c_str());
    pLabelDownloadProgress->SetSize(CVector2D(pLabelDownloadProgress->GetTextExtent() + 10, 20));

    pLabelDownloadProgressF->SetText( MbProgress_text.c_str() );
    pLabelDownloadProgressF->SetSize( CVector2D( pLabelDownloadProgressF->GetTextExtent() + 5, 20 ) );
    pLabelDownloadProgressF->SetPosition(CVector2D(screenSize.fX - (pLabelDownloadProgressF->GetTextExtent() + 40.0f), screenSize.fY - 25 * 2.70));
}

void CTransferBox::CreateTransferWindow()
{
    // Find our largest piece of text, so we can size accordingly
    std::string largeTextSample = m_titleProgressPrefix + " " + SString(_("%s of %s"), "999.99 kB", "999.99 kB");
    float       fTransferBoxWidth = m_GUI->GetTextExtent(largeTextSample.c_str(), "default-bold-small");
    fTransferBoxWidth = std::max<float>(fTransferBoxWidth, m_GUI->GetTextExtent(_("Disconnect to cancel download"), "default-normal"));

    // Add some padding to our text for the size of the window
    fTransferBoxWidth += 80;

    CVector2D screenSize = m_GUI->GetResolution();

    m_window.reset(m_GUI->CreateWnd());
    m_window->SetText("");
    m_window->SetAlpha(0.0f);
    m_window->SetVisible(false);
    m_window->SetAlwaysOnTop(true);
    m_window->SetCloseButtonEnabled(false);
    m_window->SetSizingEnabled(false);
    //m_window->SetPosition(CVector2D(screenSize.fX * 0.5f - fTransferBoxWidth * 0.5f, screenSize.fY * 0.85f - TRANSFERBOX_HEIGHT * 0.5f));
    m_window->SetPosition(CVector2D(screenSize.fX * 0.6f - fTransferBoxWidth * 0.5f, screenSize.fY * 0.85f - TRANSFERBOX_HEIGHT * 0.5f));
    m_window->SetSize(CVector2D(screenSize.fX * 0.5f - 0.2f, 58));            // relative 0.35, 0.225

    m_progressBar.reset(m_GUI->CreateProgressBar(m_window.get()));
    m_progressBar->SetPosition(CVector2D(0, TRANSFERBOX_YSTART));
    //m_progressBar->SetPosition(CVector2D(screenSize.fX * 0.6f - fTransferBoxWidth * 0.5f, screenSize.fY * 0.60f - TRANSFERBOX_HEIGHT * 0.5f));
    //m_progressBar->SetSize(CVector2D(fTransferBoxWidth, TRANSFERBOX_HEIGHT - TRANSFERBOX_YSTART - TRANSFERBOX_SPACER));
    m_progressBar->SetSize(CVector2D(screenSize.fX * 0.5f - 0.2f, screenSize.fY * 0.70f));

    m_infoLabel.reset(m_GUI->CreateLabel(m_progressBar.get(), _("Disconnect to cancel download")));
    float fTempX = (m_progressBar->GetSize().fX - m_GUI->GetTextExtent(m_infoLabel->GetText().c_str()) - TRANSFERBOX_ICONSIZE - 4) * 0.5f;
    m_infoLabel->SetPosition(CVector2D(fTempX + TRANSFERBOX_ICONSIZE + 4, 0));
    m_infoLabel->SetSize(CVector2D(fTransferBoxWidth, TRANSFERBOX_PROGRESSHEIGHT));
    m_infoLabel->SetTextColor(0, 0, 0);
    m_infoLabel->SetVerticalAlign(CGUI_ALIGN_VERTICALCENTER);

    /* for (size_t i = 0; i < m_iconImages.size(); ++i)
    {
        SString filePath("cgui\\images\\transferset\\%u.png", i + 1);
        m_iconImages[i].reset(m_GUI->CreateStaticImage(m_progressBar.get()));
        m_iconImages[i]->SetFrameEnabled(false);
        m_iconImages[i]->SetPosition(CVector2D(fTempX, ((TRANSFERBOX_PROGRESSHEIGHT) / 2) - (TRANSFERBOX_ICONSIZE / 2)));
        m_iconImages[i]->SetSize(CVector2D(TRANSFERBOX_ICONSIZE, TRANSFERBOX_ICONSIZE));
        m_iconImages[i]->LoadFromFile(filePath);
        m_iconImages[i]->SetVisible(false);
    }

    m_iconIndex = 0;
    m_iconImages[m_iconIndex]->SetVisible(true);*/
}

void CTransferBox::Show()
{
    m_visible.set(TB_VISIBILITY_MTA);
    UpdateWindowVisibility();
}

void CTransferBox::Hide()
{
    m_visible.reset(TB_VISIBILITY_MTA);
    UpdateWindowVisibility();

    m_downloadTotalSize = 0;
}

/* void CTransferBox::SetDownloadProgress(uint64_t downloadedSizeTotal)
{
    SString current = GetDataUnit(downloadedSizeTotal);
    SString total = GetDataUnit(m_downloadTotalSize);
    SString progress = m_titleProgressPrefix + " " + SString(_("%s of %s"), current.c_str(), total.c_str());
    m_window->SetText(progress.c_str());
    m_progressBar->SetProgress(static_cast<float>(static_cast<double>(downloadedSizeTotal) / m_downloadTotalSize));
*/

void CTransferBox::DoPulse()
{
    if (m_iconTimer.Get() >= 50)
    {
        m_iconTimer.Reset();
        //m_iconImages[m_iconIndex]->SetVisible(false);
        //m_iconIndex = (m_iconIndex + 1) % m_iconImages.size();
        //m_iconImages[m_iconIndex]->SetVisible(true);
    }
}

bool CTransferBox::SetClientVisibility(bool visible)
{
    if (m_visible[TB_VISIBILITY_CLIENT_SCRIPT] == visible)
        return false;

    m_visible.set(TB_VISIBILITY_CLIENT_SCRIPT, visible);
    UpdateWindowVisibility();
    return true;
}

bool CTransferBox::SetServerVisibility(bool visible)
{
    if (m_visible[TB_VISIBILITY_SERVER_SCRIPT] == visible)
        return false;

    m_visible.set(TB_VISIBILITY_SERVER_SCRIPT, visible);
    UpdateWindowVisibility();
    return true;
}

bool CTransferBox::SetAlwaysVisible(bool visible)
{
    if (m_alwaysVisible == visible)
        return false;

    m_alwaysVisible = visible;
    UpdateWindowVisibility();
    return true;
}

void CTransferBox::UpdateWindowVisibility() const
{
    bool visible = m_visible.all() || (m_visible[TB_VISIBILITY_MTA] && m_alwaysVisible);
    //m_window->SetVisible(visible);
    pProgressBG->SetVisible(visible);
    pProgressMask->SetVisible(visible);
    pLabelDownloadProgress->SetVisible(visible);
    pLabelDownloadProgressF->SetVisible(visible);
}
