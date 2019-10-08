#pragma once 

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Drawing::Imaging;

#include <math.h>
#include "ColourSpaceForm.h"

namespace SpritePlotter
{
	/// <summary> 
	/// Summary for SpritePlotterForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class SpritePlotterForm: public System::Windows::Forms::Form
	{

	///////////////////////////////////////////
	// Construction/Disposal

	public:
		SpritePlotterForm( void )
		{
			InitializeComponent();
			
			// Initialise an offscreen Bitmap to store the contents cell window, then initialise
			// a Graphics object for drawing to this bitmap
			m_bmpCellWindow = gcnew Bitmap( pbxCellWindow->ClientSize.Width,
			                                pbxCellWindow->ClientSize.Height,
			                                PixelFormat::Format32bppArgb );
			m_gfxCellWindow = Graphics::FromImage( m_bmpCellWindow );
			pbxCellWindow->Image = m_bmpCellWindow;
			
			// Create another Graphics object for drawing directly to the client area of the cell
			// window. Such drawing is only transient (i.e. is not stored and will not be
			// repainted). This is used for rubber-banding lines, rectangles etc.
			m_gfxCellWindowTransient = pbxCellWindow->CreateGraphics();
			
			// Create the main array of cells for display in the cell window - its dimensions are
			// not necessarily related to the cell window
			m_aCells = gcnew array<Color, 2>(m_kiCellArrayHeight, m_kiCellArrayWidth);
			ClearCells();
			
			// Set initial cell window offset (initially shows top-left of cell array)
			m_fCellWindowOffsetX = 0.0f;
			m_fCellWindowOffsetY = 0.0f;
			
			// Draw the initial view of the cell array
			UpdateCellWindow();
			
			// Set initial number of sides
			m_iSides = Convert::ToInt32( tbxSides->Text );
			tbbStar->Enabled = ((m_iSides & 1) && m_iSides > 3);
			if (!tbbStar->Enabled && tbbStar->Pushed)
			{
				tbbPolygon->Pushed = true;
				tbbStar->Pushed = false;
			}
			
			// No initial bitmap loaded
			m_bmpTool = nullptr;
			pbxBitmapPreview->Image = m_bmpTool;
			cmbSpriteStyle->SelectedIndex = 0;
			
			// Display initial tool settings
			ShowToolSettings();
		}

	protected:
		~SpritePlotterForm()
		{
			// Release resources
			delete m_gfxCellWindowTransient;
			delete m_gfxCellWindow;
			delete components;
		}


	///////////////////////////////////////////
	// .NET Form variables

	private:
		System::Windows::Forms::PictureBox^ pbxCellWindow;
		System::Windows::Forms::StatusBar^ sbMain;
		System::Windows::Forms::StatusBarPanel^ statusBarPanel1;
		System::Windows::Forms::StatusBarPanel^ sbpXPos;
		System::Windows::Forms::StatusBarPanel^ statusBarPanel2;
		System::Windows::Forms::StatusBarPanel^ sbpYPos;
		System::Windows::Forms::StatusBarPanel^ statusBarPanel3;
		System::Windows::Forms::ImageList^ ilToolBar;
		System::Windows::Forms::ToolBar^ tbMain;
		System::Windows::Forms::ToolBarButton^ tbbDrag;
		System::Windows::Forms::ToolBarButton^ tbbSeperator1;
		System::Windows::Forms::ToolBarButton^ tbbPoint;
		System::Windows::Forms::ToolBarButton^ tbbFreehand;
		System::Windows::Forms::ToolBarButton^ tbbLine;
		System::Windows::Forms::ToolBarButton^ tbbSeperator2;
		System::Windows::Forms::ToolBarButton^ tbbRectangle;
		System::Windows::Forms::ToolBarButton^ tbbPolygon;
		System::Windows::Forms::ToolBarButton^ tbbStar;
		System::Windows::Forms::ToolBarButton^ tbbCircle;
		System::Windows::Forms::ToolBarButton^ tbbSeperator3;
		System::Windows::Forms::ToolBarButton^ tbbBitmap;
		System::Windows::Forms::ToolBarButton^ tbbSprite;
		System::Windows::Forms::GroupBox^ groupBox1;
		System::Windows::Forms::Button^ btnClear;
		System::Windows::Forms::Label^ label1;
		System::Windows::Forms::TrackBar^ trkPixelSize;
		System::Windows::Forms::CheckBox^ cbxGrid;
		System::Windows::Forms::GroupBox^ groupBox2;
		System::Windows::Forms::Label^ label2;
		System::Windows::Forms::Label^ label3;
		System::Windows::Forms::PictureBox^ pbxFGColour;
		System::Windows::Forms::PictureBox^ pbxBGColour;
		System::Windows::Forms::Button^ btnLoadBackground;
		System::Windows::Forms::GroupBox^ gbxToolSettings;
		System::Windows::Forms::Label^ lblSides;
		System::Windows::Forms::TextBox^ tbxSides;
		System::Windows::Forms::PictureBox^ pbxBitmapPreview;
		System::Windows::Forms::Button^ btnLoadBitmap;
		System::Windows::Forms::Label^ lblStyle;
		System::Windows::Forms::ComboBox^ cmbSpriteStyle;
		System::Windows::Forms::OpenFileDialog^ dlgLoadBitmap;
		System::ComponentModel::IContainer^ components;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent( void )
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(SpritePlotterForm::typeid));
			this->sbMain = (gcnew System::Windows::Forms::StatusBar());
			this->statusBarPanel1 = (gcnew System::Windows::Forms::StatusBarPanel());
			this->sbpXPos = (gcnew System::Windows::Forms::StatusBarPanel());
			this->statusBarPanel2 = (gcnew System::Windows::Forms::StatusBarPanel());
			this->sbpYPos = (gcnew System::Windows::Forms::StatusBarPanel());
			this->statusBarPanel3 = (gcnew System::Windows::Forms::StatusBarPanel());
			this->tbMain = (gcnew System::Windows::Forms::ToolBar());
			this->tbbDrag = (gcnew System::Windows::Forms::ToolBarButton());
			this->tbbSeperator1 = (gcnew System::Windows::Forms::ToolBarButton());
			this->tbbPoint = (gcnew System::Windows::Forms::ToolBarButton());
			this->tbbFreehand = (gcnew System::Windows::Forms::ToolBarButton());
			this->tbbLine = (gcnew System::Windows::Forms::ToolBarButton());
			this->tbbSeperator2 = (gcnew System::Windows::Forms::ToolBarButton());
			this->tbbRectangle = (gcnew System::Windows::Forms::ToolBarButton());
			this->tbbPolygon = (gcnew System::Windows::Forms::ToolBarButton());
			this->tbbStar = (gcnew System::Windows::Forms::ToolBarButton());
			this->tbbCircle = (gcnew System::Windows::Forms::ToolBarButton());
			this->tbbSeperator3 = (gcnew System::Windows::Forms::ToolBarButton());
			this->tbbBitmap = (gcnew System::Windows::Forms::ToolBarButton());
			this->tbbSprite = (gcnew System::Windows::Forms::ToolBarButton());
			this->ilToolBar = (gcnew System::Windows::Forms::ImageList(this->components));
			this->trkPixelSize = (gcnew System::Windows::Forms::TrackBar());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->cbxGrid = (gcnew System::Windows::Forms::CheckBox());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->btnClear = (gcnew System::Windows::Forms::Button());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->btnLoadBackground = (gcnew System::Windows::Forms::Button());
			this->pbxFGColour = (gcnew System::Windows::Forms::PictureBox());
			this->pbxBGColour = (gcnew System::Windows::Forms::PictureBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->pbxCellWindow = (gcnew System::Windows::Forms::PictureBox());
			this->gbxToolSettings = (gcnew System::Windows::Forms::GroupBox());
			this->lblStyle = (gcnew System::Windows::Forms::Label());
			this->cmbSpriteStyle = (gcnew System::Windows::Forms::ComboBox());
			this->btnLoadBitmap = (gcnew System::Windows::Forms::Button());
			this->tbxSides = (gcnew System::Windows::Forms::TextBox());
			this->lblSides = (gcnew System::Windows::Forms::Label());
			this->pbxBitmapPreview = (gcnew System::Windows::Forms::PictureBox());
			this->dlgLoadBitmap = (gcnew System::Windows::Forms::OpenFileDialog());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->statusBarPanel1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->sbpXPos))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->statusBarPanel2))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->sbpYPos))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->statusBarPanel3))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trkPixelSize))->BeginInit();
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pbxFGColour))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pbxBGColour))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pbxCellWindow))->BeginInit();
			this->gbxToolSettings->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pbxBitmapPreview))->BeginInit();
			this->SuspendLayout();
			// 
			// sbMain
			// 
			this->sbMain->Location = System::Drawing::Point(0, 522);
			this->sbMain->Name = L"sbMain";
			this->sbMain->Panels->AddRange(gcnew cli::array< System::Windows::Forms::StatusBarPanel^  >(5) {this->statusBarPanel1, this->sbpXPos, 
				this->statusBarPanel2, this->sbpYPos, this->statusBarPanel3});
			this->sbMain->ShowPanels = true;
			this->sbMain->Size = System::Drawing::Size(640, 22);
			this->sbMain->SizingGrip = false;
			this->sbMain->TabIndex = 4;
			// 
			// statusBarPanel1
			// 
			this->statusBarPanel1->Alignment = System::Windows::Forms::HorizontalAlignment::Right;
			this->statusBarPanel1->AutoSize = System::Windows::Forms::StatusBarPanelAutoSize::Contents;
			this->statusBarPanel1->Name = L"statusBarPanel1";
			this->statusBarPanel1->Text = L"X:";
			this->statusBarPanel1->Width = 25;
			// 
			// sbpXPos
			// 
			this->sbpXPos->BorderStyle = System::Windows::Forms::StatusBarPanelBorderStyle::Raised;
			this->sbpXPos->Name = L"sbpXPos";
			this->sbpXPos->Text = L"0";
			this->sbpXPos->Width = 30;
			// 
			// statusBarPanel2
			// 
			this->statusBarPanel2->Alignment = System::Windows::Forms::HorizontalAlignment::Right;
			this->statusBarPanel2->AutoSize = System::Windows::Forms::StatusBarPanelAutoSize::Contents;
			this->statusBarPanel2->Name = L"statusBarPanel2";
			this->statusBarPanel2->Text = L"Y:";
			this->statusBarPanel2->Width = 25;
			// 
			// sbpYPos
			// 
			this->sbpYPos->BorderStyle = System::Windows::Forms::StatusBarPanelBorderStyle::Raised;
			this->sbpYPos->Name = L"sbpYPos";
			this->sbpYPos->Text = L"0";
			this->sbpYPos->Width = 30;
			// 
			// statusBarPanel3
			// 
			this->statusBarPanel3->AutoSize = System::Windows::Forms::StatusBarPanelAutoSize::Spring;
			this->statusBarPanel3->Name = L"statusBarPanel3";
			this->statusBarPanel3->Width = 530;
			// 
			// tbMain
			// 
			this->tbMain->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->tbMain->Buttons->AddRange(gcnew cli::array< System::Windows::Forms::ToolBarButton^  >(13) {this->tbbDrag, this->tbbSeperator1, 
				this->tbbPoint, this->tbbFreehand, this->tbbLine, this->tbbSeperator2, this->tbbRectangle, this->tbbPolygon, this->tbbStar, this->tbbCircle, 
				this->tbbSeperator3, this->tbbBitmap, this->tbbSprite});
			this->tbMain->DropDownArrows = true;
			this->tbMain->ImageList = this->ilToolBar;
			this->tbMain->Location = System::Drawing::Point(0, 0);
			this->tbMain->Name = L"tbMain";
			this->tbMain->ShowToolTips = true;
			this->tbMain->Size = System::Drawing::Size(640, 38);
			this->tbMain->TabIndex = 0;
			this->tbMain->ButtonClick += gcnew System::Windows::Forms::ToolBarButtonClickEventHandler(this, &SpritePlotterForm::tbMain_ButtonClick);
			// 
			// tbbDrag
			// 
			this->tbbDrag->ImageIndex = 0;
			this->tbbDrag->Name = L"tbbDrag";
			this->tbbDrag->Style = System::Windows::Forms::ToolBarButtonStyle::ToggleButton;
			this->tbbDrag->ToolTipText = L"Drag Tool";
			// 
			// tbbSeperator1
			// 
			this->tbbSeperator1->Name = L"tbbSeperator1";
			this->tbbSeperator1->Style = System::Windows::Forms::ToolBarButtonStyle::Separator;
			// 
			// tbbPoint
			// 
			this->tbbPoint->ImageIndex = 1;
			this->tbbPoint->Name = L"tbbPoint";
			this->tbbPoint->Pushed = true;
			this->tbbPoint->Style = System::Windows::Forms::ToolBarButtonStyle::ToggleButton;
			this->tbbPoint->ToolTipText = L"Point Tool";
			// 
			// tbbFreehand
			// 
			this->tbbFreehand->ImageIndex = 2;
			this->tbbFreehand->Name = L"tbbFreehand";
			this->tbbFreehand->Style = System::Windows::Forms::ToolBarButtonStyle::ToggleButton;
			this->tbbFreehand->ToolTipText = L"Freehand Tool";
			// 
			// tbbLine
			// 
			this->tbbLine->ImageIndex = 3;
			this->tbbLine->Name = L"tbbLine";
			this->tbbLine->Style = System::Windows::Forms::ToolBarButtonStyle::ToggleButton;
			this->tbbLine->ToolTipText = L"Line Tool";
			// 
			// tbbSeperator2
			// 
			this->tbbSeperator2->Name = L"tbbSeperator2";
			this->tbbSeperator2->Style = System::Windows::Forms::ToolBarButtonStyle::Separator;
			// 
			// tbbRectangle
			// 
			this->tbbRectangle->ImageIndex = 4;
			this->tbbRectangle->Name = L"tbbRectangle";
			this->tbbRectangle->Style = System::Windows::Forms::ToolBarButtonStyle::ToggleButton;
			this->tbbRectangle->ToolTipText = L"Rectangle Tool";
			// 
			// tbbPolygon
			// 
			this->tbbPolygon->ImageIndex = 5;
			this->tbbPolygon->Name = L"tbbPolygon";
			this->tbbPolygon->Style = System::Windows::Forms::ToolBarButtonStyle::ToggleButton;
			this->tbbPolygon->ToolTipText = L"Polygon Tool";
			// 
			// tbbStar
			// 
			this->tbbStar->ImageIndex = 6;
			this->tbbStar->Name = L"tbbStar";
			this->tbbStar->Style = System::Windows::Forms::ToolBarButtonStyle::ToggleButton;
			this->tbbStar->ToolTipText = L"Star Tool";
			// 
			// tbbCircle
			// 
			this->tbbCircle->ImageIndex = 7;
			this->tbbCircle->Name = L"tbbCircle";
			this->tbbCircle->Style = System::Windows::Forms::ToolBarButtonStyle::ToggleButton;
			this->tbbCircle->ToolTipText = L"Circle Tool";
			// 
			// tbbSeperator3
			// 
			this->tbbSeperator3->Name = L"tbbSeperator3";
			this->tbbSeperator3->Style = System::Windows::Forms::ToolBarButtonStyle::Separator;
			// 
			// tbbBitmap
			// 
			this->tbbBitmap->ImageIndex = 8;
			this->tbbBitmap->Name = L"tbbBitmap";
			this->tbbBitmap->Style = System::Windows::Forms::ToolBarButtonStyle::ToggleButton;
			this->tbbBitmap->ToolTipText = L"Bitmap Tool";
			// 
			// tbbSprite
			// 
			this->tbbSprite->ImageIndex = 9;
			this->tbbSprite->Name = L"tbbSprite";
			this->tbbSprite->Style = System::Windows::Forms::ToolBarButtonStyle::ToggleButton;
			this->tbbSprite->ToolTipText = L"Sprite Tool";
			// 
			// ilToolBar
			// 
			this->ilToolBar->ImageStream = (cli::safe_cast<System::Windows::Forms::ImageListStreamer^  >(resources->GetObject(L"ilToolBar.ImageStream")));
			this->ilToolBar->TransparentColor = System::Drawing::Color::White;
			this->ilToolBar->Images->SetKeyName(0, L"");
			this->ilToolBar->Images->SetKeyName(1, L"");
			this->ilToolBar->Images->SetKeyName(2, L"");
			this->ilToolBar->Images->SetKeyName(3, L"");
			this->ilToolBar->Images->SetKeyName(4, L"");
			this->ilToolBar->Images->SetKeyName(5, L"");
			this->ilToolBar->Images->SetKeyName(6, L"");
			this->ilToolBar->Images->SetKeyName(7, L"");
			this->ilToolBar->Images->SetKeyName(8, L"");
			this->ilToolBar->Images->SetKeyName(9, L"");
			// 
			// trkPixelSize
			// 
			this->trkPixelSize->AutoSize = false;
			this->trkPixelSize->Location = System::Drawing::Point(40, 16);
			this->trkPixelSize->Maximum = 25;
			this->trkPixelSize->Minimum = 1;
			this->trkPixelSize->Name = L"trkPixelSize";
			this->trkPixelSize->Size = System::Drawing::Size(96, 32);
			this->trkPixelSize->TabIndex = 1;
			this->trkPixelSize->TickFrequency = 4;
			this->trkPixelSize->Value = 3;
			this->trkPixelSize->Scroll += gcnew System::EventHandler(this, &SpritePlotterForm::trkPixelSize_Scroll);
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(8, 24);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(40, 16);
			this->label1->TabIndex = 0;
			this->label1->Text = L"Zoom";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// cbxGrid
			// 
			this->cbxGrid->Location = System::Drawing::Point(96, 56);
			this->cbxGrid->Name = L"cbxGrid";
			this->cbxGrid->Size = System::Drawing::Size(45, 24);
			this->cbxGrid->TabIndex = 3;
			this->cbxGrid->Text = L"Grid";
			this->cbxGrid->CheckedChanged += gcnew System::EventHandler(this, &SpritePlotterForm::cbxGrid_CheckedChanged);
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->btnClear);
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Controls->Add(this->cbxGrid);
			this->groupBox1->Controls->Add(this->trkPixelSize);
			this->groupBox1->Location = System::Drawing::Point(488, 40);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(144, 88);
			this->groupBox1->TabIndex = 1;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Drawing Area";
			// 
			// btnClear
			// 
			this->btnClear->Location = System::Drawing::Point(8, 56);
			this->btnClear->Name = L"btnClear";
			this->btnClear->Size = System::Drawing::Size(72, 23);
			this->btnClear->TabIndex = 2;
			this->btnClear->Text = L"Clear";
			this->btnClear->Click += gcnew System::EventHandler(this, &SpritePlotterForm::btnClear_Click);
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->btnLoadBackground);
			this->groupBox2->Controls->Add(this->pbxFGColour);
			this->groupBox2->Controls->Add(this->pbxBGColour);
			this->groupBox2->Controls->Add(this->label3);
			this->groupBox2->Controls->Add(this->label2);
			this->groupBox2->Location = System::Drawing::Point(488, 136);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(144, 128);
			this->groupBox2->TabIndex = 2;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Colours";
			// 
			// btnLoadBackground
			// 
			this->btnLoadBackground->Location = System::Drawing::Point(8, 96);
			this->btnLoadBackground->Name = L"btnLoadBackground";
			this->btnLoadBackground->Size = System::Drawing::Size(128, 23);
			this->btnLoadBackground->TabIndex = 2;
			this->btnLoadBackground->Text = L"Load Background...";
			this->btnLoadBackground->Click += gcnew System::EventHandler(this, &SpritePlotterForm::btnLoadBackground_Click);
			// 
			// pbxFGColour
			// 
			this->pbxFGColour->BackColor = System::Drawing::Color::Black;
			this->pbxFGColour->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->pbxFGColour->Location = System::Drawing::Point(104, 16);
			this->pbxFGColour->Name = L"pbxFGColour";
			this->pbxFGColour->Size = System::Drawing::Size(32, 32);
			this->pbxFGColour->TabIndex = 0;
			this->pbxFGColour->TabStop = false;
			this->pbxFGColour->Click += gcnew System::EventHandler(this, &SpritePlotterForm::pbxFGColour_Click);
			// 
			// pbxBGColour
			// 
			this->pbxBGColour->BackColor = System::Drawing::Color::White;
			this->pbxBGColour->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->pbxBGColour->Location = System::Drawing::Point(104, 56);
			this->pbxBGColour->Name = L"pbxBGColour";
			this->pbxBGColour->Size = System::Drawing::Size(32, 32);
			this->pbxBGColour->TabIndex = 0;
			this->pbxBGColour->TabStop = false;
			this->pbxBGColour->Click += gcnew System::EventHandler(this, &SpritePlotterForm::pbxBGColour_Click);
			// 
			// label3
			// 
			this->label3->Location = System::Drawing::Point(8, 64);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(72, 16);
			this->label3->TabIndex = 1;
			this->label3->Text = L"Background";
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(8, 24);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(72, 16);
			this->label2->TabIndex = 0;
			this->label2->Text = L"Foreground";
			// 
			// pbxCellWindow
			// 
			this->pbxCellWindow->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->pbxCellWindow->Cursor = System::Windows::Forms::Cursors::Cross;
			this->pbxCellWindow->Location = System::Drawing::Point(2, 40);
			this->pbxCellWindow->Name = L"pbxCellWindow";
			this->pbxCellWindow->Size = System::Drawing::Size(482, 482);
			this->pbxCellWindow->TabIndex = 7;
			this->pbxCellWindow->TabStop = false;
			this->pbxCellWindow->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &SpritePlotterForm::pbxCellWindow_MouseMove);
			this->pbxCellWindow->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &SpritePlotterForm::pbxCellWindow_MouseDown);
			this->pbxCellWindow->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &SpritePlotterForm::pbxCellWindow_MouseUp);
			// 
			// gbxToolSettings
			// 
			this->gbxToolSettings->Controls->Add(this->lblStyle);
			this->gbxToolSettings->Controls->Add(this->cmbSpriteStyle);
			this->gbxToolSettings->Controls->Add(this->btnLoadBitmap);
			this->gbxToolSettings->Controls->Add(this->tbxSides);
			this->gbxToolSettings->Controls->Add(this->lblSides);
			this->gbxToolSettings->Controls->Add(this->pbxBitmapPreview);
			this->gbxToolSettings->Location = System::Drawing::Point(488, 272);
			this->gbxToolSettings->Name = L"gbxToolSettings";
			this->gbxToolSettings->Size = System::Drawing::Size(144, 208);
			this->gbxToolSettings->TabIndex = 3;
			this->gbxToolSettings->TabStop = false;
			this->gbxToolSettings->Text = L"Tool Settings";
			// 
			// lblStyle
			// 
			this->lblStyle->Location = System::Drawing::Point(8, 180);
			this->lblStyle->Name = L"lblStyle";
			this->lblStyle->Size = System::Drawing::Size(32, 16);
			this->lblStyle->TabIndex = 3;
			this->lblStyle->Text = L"Style";
			// 
			// cmbSpriteStyle
			// 
			this->cmbSpriteStyle->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cmbSpriteStyle->Items->AddRange(gcnew cli::array< System::Object^  >(5) {L"Cutout", L"Alpha", L"Add", L"Multiply", 
				L"Luminosity"});
			this->cmbSpriteStyle->Location = System::Drawing::Point(40, 176);
			this->cmbSpriteStyle->Name = L"cmbSpriteStyle";
			this->cmbSpriteStyle->Size = System::Drawing::Size(96, 21);
			this->cmbSpriteStyle->TabIndex = 4;
			// 
			// btnLoadBitmap
			// 
			this->btnLoadBitmap->Location = System::Drawing::Point(8, 144);
			this->btnLoadBitmap->Name = L"btnLoadBitmap";
			this->btnLoadBitmap->Size = System::Drawing::Size(128, 23);
			this->btnLoadBitmap->TabIndex = 2;
			this->btnLoadBitmap->Text = L"Load Bitmap/Sprite...";
			this->btnLoadBitmap->Click += gcnew System::EventHandler(this, &SpritePlotterForm::btnLoadBitmap_Click);
			// 
			// tbxSides
			// 
			this->tbxSides->Location = System::Drawing::Point(104, 16);
			this->tbxSides->Name = L"tbxSides";
			this->tbxSides->Size = System::Drawing::Size(32, 20);
			this->tbxSides->TabIndex = 1;
			this->tbxSides->Text = L"5";
			this->tbxSides->TextChanged += gcnew System::EventHandler(this, &SpritePlotterForm::tbxSides_TextChanged);
			// 
			// lblSides
			// 
			this->lblSides->Location = System::Drawing::Point(8, 20);
			this->lblSides->Name = L"lblSides";
			this->lblSides->Size = System::Drawing::Size(88, 16);
			this->lblSides->TabIndex = 0;
			this->lblSides->Text = L"Number of Sides";
			// 
			// pbxBitmapPreview
			// 
			this->pbxBitmapPreview->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->pbxBitmapPreview->Location = System::Drawing::Point(9, 16);
			this->pbxBitmapPreview->Name = L"pbxBitmapPreview";
			this->pbxBitmapPreview->Size = System::Drawing::Size(126, 122);
			this->pbxBitmapPreview->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->pbxBitmapPreview->TabIndex = 2;
			this->pbxBitmapPreview->TabStop = false;
			// 
			// dlgLoadBitmap
			// 
			this->dlgLoadBitmap->DefaultExt = L"bmp";
			this->dlgLoadBitmap->Filter = L"Bitmap files (*.bmp,*.jpg,*.png,*.gif,*.tif)|*.bmp;*.jpg;*.png;*.gif;*.tif|All fi" 
				L"les|*.*";
			this->dlgLoadBitmap->Title = L"Load Bitmap";
			// 
			// SpritePlotterForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(640, 544);
			this->Controls->Add(this->pbxCellWindow);
			this->Controls->Add(this->tbMain);
			this->Controls->Add(this->sbMain);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->gbxToolSettings);
			this->Cursor = System::Windows::Forms::Cursors::Arrow;
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->Location = System::Drawing::Point(0, 38);
			this->MaximizeBox = false;
			this->Name = L"SpritePlotterForm";
			this->Text = L"Sprite Plotter";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->statusBarPanel1))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->sbpXPos))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->statusBarPanel2))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->sbpYPos))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->statusBarPanel3))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trkPixelSize))->EndInit();
			this->groupBox1->ResumeLayout(false);
			this->groupBox2->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pbxFGColour))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pbxBGColour))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pbxCellWindow))->EndInit();
			this->gbxToolSettings->ResumeLayout(false);
			this->gbxToolSettings->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pbxBitmapPreview))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion


	///////////////////////////////////////////
	// Form events
	private:

		System::Void pbxCellWindow_MouseDown( System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
		{
			m_iWindowDownX = e->X;
			m_iWindowDownY = e->Y;
			m_iCellDownX = CellXFromWindowX( e->X );
			m_iCellDownY = CellYFromWindowY( e->Y );
			
			// Drawing actions are based on mouse button use
			switch (e->Button)
			{
				case ::MouseButtons::Left:
				case ::MouseButtons::Right:
					
					// Get drawing colour
					Color colourDraw;
					if (e->Button == ::MouseButtons::Left)
					{
						colourDraw = FGColour();
					}
					else
					{
						colourDraw = BGColour();
					}
					
					// Interpret initial mouse click based on which tool is in use
					if (tbbPoint->Pushed || tbbFreehand->Pushed)
					{
						SetViewportPixel( m_iCellDownX, m_iCellDownY, colourDraw );
						UpdateCellWindow();
					}
					else if ( tbbBitmap->Pushed || tbbSprite->Pushed )
					{
						DrawGuideLines( e->X, e->Y );
					}
					break;
			}
		}

		System::Void pbxCellWindow_MouseMove( System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
		{
			int iCellMoveX = CellXFromWindowX( e->X );
			int iCellMoveY = CellYFromWindowY( e->Y );
			
			// Update status bar text
			sbpXPos->Text = Convert::ToString( iCellMoveX );
			sbpYPos->Text = Convert::ToString( iCellMoveY );
			
			// Drawing actions are based on mouse button use
			switch (e->Button)
			{
				case ::MouseButtons::Left:
				case ::MouseButtons::Right:
				{
					// Get drawing colour
					Color colourDraw;
					if (e->Button == ::MouseButtons::Left)
					{
						colourDraw = FGColour();
					}
					else
					{
						colourDraw = BGColour();
					}
					
					// Interpret mouse movement based on which tool is in use
					if (tbbDrag->Pushed)
					{
						
						// Update cell window offset based on latest mouse movement
						m_fCellWindowOffsetX -= static_cast<float>(e->X - m_iWindowDownX) / CellWidth();
						m_fCellWindowOffsetY -= static_cast<float>(e->Y - m_iWindowDownY) / CellHeight();
						CheckCellWindowBounds();
						
						// Redraw entire cell window
						UpdateCellWindow();
						
						// Reset mouse down event to latest position
						m_iWindowDownX = e->X;
						m_iWindowDownY = e->Y;
						m_iCellDownX = iCellMoveX;
						m_iCellDownY = iCellMoveY;
					}
					else if (tbbFreehand->Pushed)
					{
						SetViewportPixel( iCellMoveX, iCellMoveY, colourDraw );
// Replace the above line with the line below for a better freehand tool once the DrawLine
// function is working
//						DrawLine( m_iCellDownX, m_iCellDownY, iCellMoveX, iCellMoveY, colourDraw );
						m_iWindowDownX = e->X;
						m_iWindowDownY = e->Y;
						m_iCellDownX = iCellMoveX;
						m_iCellDownY = iCellMoveY;
						UpdateCellWindow();
					}
					else if (tbbLine->Pushed || tbbRectangle->Pushed || tbbPolygon->Pushed ||
					         tbbStar->Pushed || tbbCircle->Pushed ||
					         tbbBitmap->Pushed || tbbSprite->Pushed )
					{
						DrawGuideLines( e->X, e->Y );
					}
					break;
				}

				case ::MouseButtons::Middle:
					pbxCellWindow->Cursor = Cursors::Hand;
					
					// Update cell window offset based on latest mouse movement
					m_fCellWindowOffsetX -= static_cast<float>(e->X - m_iWindowDownX) / CellWidth();
					m_fCellWindowOffsetY -= static_cast<float>(e->Y - m_iWindowDownY) / CellHeight();
					CheckCellWindowBounds();
					
					// Redraw entire cell window
					UpdateCellWindow();
					
					// Reset mouse down event to latest position
					m_iWindowDownX = e->X;
					m_iWindowDownY = e->Y;
					m_iCellDownX = iCellMoveX;
					m_iCellDownY = iCellMoveY;
					break;
			}
		}

		System::Void pbxCellWindow_MouseUp( System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
		{
			int iCellUpX = CellXFromWindowX( e->X );
			int iCellUpY = CellYFromWindowY( e->Y );
			
			// Drawing actions are based on mouse button use
			switch (e->Button)
			{
				case ::MouseButtons::Left:
				case ::MouseButtons::Right:
				{	
					// Get drawing colour
					Color colourDraw;
					if (e->Button == ::MouseButtons::Left)
					{
						colourDraw = FGColour();
					}
					else
					{
						colourDraw = BGColour();
					}
					
					// Interpret mouse release based on which tool is in use
					if (tbbLine->Pushed)
					{
						DrawLine( m_iCellDownX, m_iCellDownY, iCellUpX, iCellUpY, colourDraw );
						UpdateCellWindow();
					}
					else if (tbbRectangle->Pushed)
					{
						int iLeftX = Math::Min( m_iCellDownX, iCellUpX );
						int iRightX = Math::Max( m_iCellDownX, iCellUpX );
						int iTopY = Math::Min( m_iCellDownY, iCellUpY );
						int iBottomY = Math::Max( m_iCellDownY, iCellUpY );
						DrawRectangle( iLeftX, iTopY, iRightX - iLeftX + 1, iBottomY - iTopY + 1, colourDraw );
						UpdateCellWindow();
					}
					else if (tbbPolygon->Pushed)
					{
						int iMoveX = iCellUpX - m_iCellDownX;
						int iMoveY = iCellUpY - m_iCellDownY;
						int iRadius = static_cast<int>(Math::Sqrt( iMoveX * iMoveX + iMoveY * iMoveY ) + 0.5f);
						DrawPolygon( m_iSides, m_iCellDownX, m_iCellDownY, iRadius, colourDraw );
						UpdateCellWindow();
					}
					else if (tbbStar->Pushed)
					{
						int iMoveX = iCellUpX - m_iCellDownX;
						int iMoveY = iCellUpY - m_iCellDownY;
						int iRadius = static_cast<int>(Math::Sqrt( iMoveX * iMoveX + iMoveY * iMoveY ) + 0.5f);
						DrawStar( m_iSides, m_iCellDownX, m_iCellDownY, iRadius, colourDraw );
						UpdateCellWindow();
					}
					else if (tbbCircle->Pushed)
					{
						int iMoveX = iCellUpX - m_iCellDownX;
						int iMoveY = iCellUpY - m_iCellDownY;
						int iRadius = static_cast<int>(Math::Sqrt( iMoveX * iMoveX + iMoveY * iMoveY ) + 0.5f);
						DrawCircle( m_iCellDownX, m_iCellDownY, iRadius, colourDraw );
						UpdateCellWindow();
					}
					else if (tbbBitmap->Pushed && m_bmpTool != nullptr)
					{
						DrawBitmap( iCellUpX, iCellUpY, m_bmpTool );
						UpdateCellWindow();
					}
					else if (tbbSprite->Pushed && m_bmpTool != nullptr)
					{
						String^ sStyle = cmbSpriteStyle->SelectedItem->ToString();
						if (sStyle == "Cutout")
						{
							DrawSprite( iCellUpX, iCellUpY, m_bmpTool );
						}
						else if (sStyle == "Alpha")
						{
							DrawSpriteAlpha( iCellUpX, iCellUpY, m_bmpTool );
						}
						else
						if (sStyle == "Add")
						{
							DrawSpriteAdd( iCellUpX, iCellUpY, m_bmpTool );
						}
						else
						if (sStyle == "Multiply")
						{
							DrawSpriteMultiply( iCellUpX, iCellUpY, m_bmpTool );
						}
						else
						if (sStyle == "Luminosity")
						{
							DrawSpriteLuminosity( iCellUpX, iCellUpY, m_bmpTool );
						}
						UpdateCellWindow();
					}
					break;
				}

				case ::MouseButtons::Middle:
					if ( tbbDrag->Pushed )
					{
						pbxCellWindow->Cursor = Cursors::Hand;
					}
					else
					{
						pbxCellWindow->Cursor = Cursors::Cross;
					}
					break;
			}
		}

		System::Void tbMain_ButtonClick( System::Object^ sender, System::Windows::Forms::ToolBarButtonClickEventArgs^ e )
		{
			tbbDrag->Pushed = false;
			tbbPoint->Pushed = false;
			tbbFreehand->Pushed = false;
			tbbLine->Pushed = false;
			tbbRectangle->Pushed = false;
			tbbPolygon->Pushed = false;
			tbbStar->Pushed = false;
			tbbCircle->Pushed = false;
			tbbBitmap->Pushed = false;
			tbbSprite->Pushed = false;
			e->Button->Pushed = true;
			if ( e->Button == tbbDrag )
			{
				pbxCellWindow->Cursor = Cursors::Hand;
			}
			else
			{
				pbxCellWindow->Cursor = Cursors::Cross;
			}

			ShowToolSettings();
		}

		System::Void btnClear_Click( System::Object^ sender, System::EventArgs^ e )
		{
			ClearCells();
			UpdateCellWindow();
		}

		System::Void trkPixelSize_Scroll( System::Object^ sender, System::EventArgs^ e )
		{
			cbxGrid->Enabled = (trkPixelSize->Value > 4);
			CheckCellWindowBounds();
			UpdateCellWindow();
		}

		System::Void cbxGrid_CheckedChanged( System::Object^ sender, System::EventArgs^ e )
		{
			UpdateCellWindow();
		}

		System::Void pbxFGColour_Click( System::Object^ sender, System::EventArgs^ e )
		{
			int iR = pbxFGColour->BackColor.R;
			int iG = pbxFGColour->BackColor.G;
			int iB = pbxFGColour->BackColor.B;
			ColourSpaceForm^ colourForm = gcnew ColourSpaceForm( iR, iG, iB );
			if (colourForm->ShowDialog() != ::DialogResult::Cancel)
			{
				pbxFGColour->BackColor = colourForm->GetColour();
			}
			delete colourForm;
		}

		System::Void pbxBGColour_Click( System::Object^ sender, System::EventArgs^ e )
		{
			int iR = pbxBGColour->BackColor.R;
			int iG = pbxBGColour->BackColor.G;
			int iB = pbxBGColour->BackColor.B;
			ColourSpaceForm^ colourForm = gcnew ColourSpaceForm( iR, iG, iB );
			if (colourForm->ShowDialog() != ::DialogResult::Cancel)
			{
				pbxBGColour->BackColor = colourForm->GetColour();
			}
			delete colourForm;
		}

		System::Void btnLoadBackground_Click( System::Object^ sender, System::EventArgs^ e )
		{
			if (dlgLoadBitmap->ShowDialog() == ::DialogResult::OK)
			{
				Bitmap^ bmpOriginal = nullptr;
				Bitmap^ bmpBackground;
				try
				{
					bmpOriginal = gcnew Bitmap( dlgLoadBitmap->FileName );
					bmpBackground = gcnew Bitmap( m_kiCellArrayWidth, m_kiCellArrayHeight, 
					                              PixelFormat::Format32bppArgb );
				}
				catch( Exception^ ) 
				{
					delete bmpOriginal;
					MessageBox::Show( L"Error loading background bitmap", L"Sprite Plotter - Error", 
					                  MessageBoxButtons::OK, MessageBoxIcon::Error );
					return;
				}

				// Convert bitmap to ARGB8888 & lock bits for more efficient access
				Graphics^ gfxBackground = Graphics::FromImage( bmpBackground );
				gfxBackground->DrawImage( bmpOriginal, 0, 0, m_kiCellArrayWidth, m_kiCellArrayHeight );
				BitmapData^ bmpData = bmpBackground->LockBits( Rectangle(0,0,m_kiCellArrayWidth,m_kiCellArrayHeight), ImageLockMode::ReadOnly, PixelFormat::Format32bppArgb );
				unsigned char * bmpPixels = static_cast<unsigned char *>(bmpData->Scan0.ToPointer());
				int offset = 0;
				for (int BitmapY = 0; BitmapY < m_kiCellArrayHeight; ++BitmapY)
				{
					for (int BitmapX = 0; BitmapX < m_kiCellArrayWidth; ++BitmapX)
					{
						unsigned char B = bmpPixels[ offset++ ];
						unsigned char G = bmpPixels[ offset++ ];
						unsigned char R = bmpPixels[ offset++ ];
						offset++; // Skip alpha
						Color pixelColour = Color::FromArgb( R, G, B );
						SetViewportPixel( BitmapX, BitmapY, pixelColour );
					}
				}
				bmpBackground->UnlockBits( bmpData );
				delete bmpBackground;
				delete bmpOriginal;
				UpdateCellWindow();
			}
		}

		System::Void tbxSides_TextChanged( System::Object^ sender, System::EventArgs^ e )
		{
			try
			{
				m_iSides = Convert::ToInt32( tbxSides->Text );
			}
			catch( Exception^ ) 
			{
				tbxSides->Text = "3";
				return;
			}

			if (m_iSides < 3)
			{
				tbxSides->Text = "3";
			}

			tbbStar->Enabled = ((m_iSides & 1) && m_iSides > 3);
			if (!tbbStar->Enabled && tbbStar->Pushed)
			{
				tbbPolygon->Pushed = true;
				tbbStar->Pushed = false;
			}
		}

		System::Void btnLoadBitmap_Click( System::Object^ sender, System::EventArgs^ e )
		{
			if (dlgLoadBitmap->ShowDialog() == ::DialogResult::OK)
			{
				try
				{
					m_bmpTool = gcnew Bitmap( dlgLoadBitmap->FileName );
				}
				catch ( Exception^ ) 
				{
					MessageBox::Show( L"Error loading bitmap file", L"Sprite Plotter - Error",
					                  MessageBoxButtons::OK, MessageBoxIcon::Error );
					m_bmpTool = nullptr;
					return;
				}

				pbxBitmapPreview->Image = m_bmpTool;
			}
		}


	///////////////////////////////////////////
	// Application specific functions
	private:

		///////////////////////////////////////////
		// Encapsulate some form controls

		// Width of the cell window in pixels
		int CellWindowWidth()
		{
			return pbxCellWindow->ClientSize.Width;
		}


		// Height of the cell window in pixels
		int CellWindowHeight()
		{
			return pbxCellWindow->ClientSize.Height;
		}


		// Current width of a cell in pixels
		int CellWidth()
		{
			return trkPixelSize->Value;
		}


		// Current height of a cell in pixels
		int CellHeight()
		{
			return trkPixelSize->Value;
		}


		// Get grid use (if grid is disabled then it is not used
		bool UseGrid()
		{
			return (cbxGrid->Checked && cbxGrid->Enabled);
		}


		// Access drawing colours
		Color FGColour()
		{
			return pbxFGColour->BackColor;
		}

		Color BGColour()
		{
			return pbxBGColour->BackColor;
		}

		Color GridColour()
		{
			return Color::Gray;
		}


		///////////////////////////////////////////
		// Control visibility

		void ShowToolSettings()
		{
			bool bShowPolygon = (tbbPolygon->Pushed || tbbStar->Pushed);
			bool bShowBitmap = (tbbBitmap->Pushed || tbbSprite->Pushed);
			gbxToolSettings->Visible = bShowPolygon | bShowBitmap;
			lblSides->Visible = bShowPolygon;
			tbxSides->Visible = bShowPolygon;
			pbxBitmapPreview->Visible = bShowBitmap;
			btnLoadBitmap->Visible = bShowBitmap;
			if (bShowPolygon)
			{
				gbxToolSettings->Text = "Polygon Settings";
			}

			if (bShowBitmap)
			{
				gbxToolSettings->Text = "Bitmap Settings";
				lblStyle->Visible = tbbSprite->Pushed;
				cmbSpriteStyle->Visible = tbbSprite->Pushed;
			}
		}


		///////////////////////////////////////////
		// Cell window offset management

		int FirstCellX()
		{
			return static_cast<int>(m_fCellWindowOffsetX);
		}

		int FirstCellY()
		{
			return static_cast<int>(m_fCellWindowOffsetY);
		}

		int FirstCellOffsetX()
		{
			return static_cast<int>(CellWidth() * (Math::Floor( m_fCellWindowOffsetX ) - m_fCellWindowOffsetX));
		}

		int FirstCellOffsetY()
		{
			return static_cast<int>(CellHeight() * (Math::Floor( m_fCellWindowOffsetY ) - m_fCellWindowOffsetY));
		}


		///////////////////////////////////////////
		// Coordinate conversions

		// Convert between window (pixel) coordinates and cell coordinates
		int CellXFromWindowX( int iWindowX )
		{
			return FirstCellX() + (iWindowX - FirstCellOffsetX()) / CellWidth();
		}

		int CellYFromWindowY( int iWindowY )
		{
			return FirstCellY() + (iWindowY - FirstCellOffsetY()) / CellHeight();
		}

		int WindowXFromCellX( int iCellX )
		{
			return FirstCellOffsetX() + (iCellX - FirstCellX()) * CellWidth();
		}

		int WindowYFromCellY( int iCellY )
		{
			return FirstCellOffsetY() + (iCellY - FirstCellY()) * CellHeight();
		}


		//////////////////////////////////
		// Guide line drawing functions

		// Draw guide lines during the mouse move event for certain tools. Draw directly to window
		// client area, using an invalidate/update sequence to remove the previous guide lines
		// using the stored bitmap (which is unaffected by the guide lines)
		void DrawGuideLines( int iCurrentX, int iCurrentY )
		{
			int iStartX = WindowXFromCellX( m_iCellDownX ) + CellWidth() / 2;
			int iStartY = WindowYFromCellY( m_iCellDownY ) + CellHeight() / 2;
			pbxCellWindow->Invalidate();
			pbxCellWindow->Update();
			Pen^ penTemp = gcnew Pen( GridColour() );
			penTemp->DashStyle = Drawing2D::DashStyle::Dash;
			if (tbbLine->Pushed)
			{
				m_gfxCellWindowTransient->DrawLine( penTemp, iStartX, iStartY, iCurrentX, iCurrentY );
			}
			else
			if (tbbRectangle->Pushed)
			{
				int iLeftX = Math::Min( iStartX, iCurrentX );
				int iRightX = Math::Max( iStartX, iCurrentX );
				int iTopY = Math::Min( iStartY, iCurrentY );
				int iBottomY = Math::Max( iStartY, iCurrentY );
				m_gfxCellWindowTransient->DrawRectangle( penTemp, iLeftX, iTopY, iRightX - iLeftX + 1, iBottomY - iTopY + 1 );
			}
			else if (tbbPolygon->Pushed || tbbStar->Pushed)
			{
				int iMoveX = iCurrentX - iStartX;
				int iMoveY = iCurrentY - iStartY;
				double radius = Math::Sqrt( iMoveX * iMoveX + iMoveY * iMoveY );
				int iSides = m_iSides;
				array<Point>^ polyPoints = gcnew array<Point>(iSides);
				double polyAngle = Math::PI * 2.0 / iSides;
				if (tbbStar->Pushed)
				{
					polyAngle *= 2.0;
				}

				for (int iPoint = 0; iPoint < iSides; ++iPoint)
				{
					polyPoints[ iPoint ].X = iStartX + static_cast<int>(radius * Math::Sin( polyAngle * iPoint ) + 0.5);
					polyPoints[ iPoint ].Y = iStartY - static_cast<int>(radius * Math::Cos( polyAngle * iPoint ) + 0.5);
				}
				m_gfxCellWindowTransient->DrawPolygon( penTemp, polyPoints );
			}
			else if (tbbCircle->Pushed)
			{
				int iMoveX = iCurrentX - iStartX;
				int iMoveY = iCurrentY - iStartY;
				int iRadius = static_cast<int>(Math::Sqrt( iMoveX * iMoveX + iMoveY * iMoveY ));
				m_gfxCellWindowTransient->DrawEllipse( penTemp, iStartX - iRadius, iStartY - iRadius, iRadius * 2, iRadius * 2 );
			}
			else if (tbbBitmap->Pushed || tbbSprite->Pushed)
			{
				if (m_bmpTool != nullptr)
				{
					m_gfxCellWindowTransient->DrawRectangle( penTemp, iCurrentX, iCurrentY, m_bmpTool->Width * CellWidth(), m_bmpTool->Height * CellHeight() );
				}
			}
		}


		//////////////////////////////////
		// Cell window drawing functions

		// Keep cell window within cell array
		void CheckCellWindowBounds()
		{
			if (m_fCellWindowOffsetX < 0.0f)
			{
				m_fCellWindowOffsetX = 0.0f;
			}

			if (m_fCellWindowOffsetY < 0.0f)
			{
				m_fCellWindowOffsetY = 0.0f;
			}

			float fCellsWidth = static_cast<float>(CellWindowWidth()) / CellWidth();
			if (m_fCellWindowOffsetX > m_kiCellArrayWidth - fCellsWidth)
			{
				m_fCellWindowOffsetX = m_kiCellArrayWidth - fCellsWidth;
			}

			float fCellsHeight = static_cast<float>(CellWindowHeight()) / CellHeight();
			if (m_fCellWindowOffsetY > m_kiCellArrayHeight - fCellsHeight)
			{
				m_fCellWindowOffsetY = m_kiCellArrayHeight - fCellsHeight;
			}
		}


		// Clear all the cells to their default value
		void ClearCells()
		{
			for (int iY = 0; iY < m_kiCellArrayHeight; ++iY)
			{
				for (int iX = 0; iX < m_kiCellArrayWidth; ++iX)
				{
					m_aCells[iY, iX] = BGColour();
				}
			}
		}


		// Redraw the entire cell window
		void UpdateCellWindow()
		{
			m_gfxCellWindow->Clear( BGColour() );
			DrawAllPixels();
			if (UseGrid())
			{
				Pen^ penGrid = gcnew Pen( GridColour() );
				for (int iX = FirstCellOffsetX(); iX < CellWindowWidth(); iX += CellWidth())
				{
					m_gfxCellWindow->DrawLine( penGrid, iX, 0, iX, CellWindowHeight() - 1 );
				}
				for (int iY = FirstCellOffsetY(); iY < CellWindowHeight(); iY += CellHeight())
				{
					m_gfxCellWindow->DrawLine( penGrid, 0, iY, CellWindowWidth() - 1, iY );
				}
			}

			pbxCellWindow->Invalidate();
			pbxCellWindow->Update();
		}


		// Redraw all the pixels visible in the cell window. Designed for efficiency.
		void DrawAllPixels()
		{
			BitmapData^ bmdCellWindow = 
				m_bmpCellWindow->LockBits( Rectangle(Point(0, 0), m_bmpCellWindow->Size),
				                           ImageLockMode::WriteOnly, PixelFormat::Format32bppArgb );
			int* pPixels = static_cast<int*>(bmdCellWindow->Scan0.ToPointer());
			int iStride = (bmdCellWindow->Stride / 4) - bmdCellWindow->Width;
			int iCellHeight = CellHeight();
			int iCellWidth = CellWidth();
			int iX = FirstCellX();
			int iY = FirstCellY();
			int iInitCellXPos = FirstCellOffsetX() + CellWidth();
			int iInitCellYPos = FirstCellOffsetY() + CellHeight();
			int iCellYPos = iInitCellYPos;
			int iPixY = 0;
			while (true)
			{
				interior_ptr<Color> CurrentCell = &m_aCells[iY, iX];
				int pixelColour = CurrentCell->ToArgb();
				int iCellXPos = iInitCellXPos;
				int iPixX = 0;
				while (true)
				{
					*pPixels = pixelColour;
					++pPixels;
					++iPixX;
					if (iPixX == bmdCellWindow->Width)
					{
						break;
					}

					--iCellXPos;
					if (!iCellXPos)
					{
						iCellXPos = iCellWidth;
						++CurrentCell;
						pixelColour = CurrentCell->ToArgb();
					}
				}

				pPixels += iStride;
				++iPixY;
				if (iPixY == bmdCellWindow->Height)
				{
					break;
				}

				--iCellYPos;
				if (!iCellYPos)
				{
					iCellYPos = iCellHeight;
					++iY;
				}
			}

			m_bmpCellWindow->UnlockBits( bmdCellWindow );
		}


		// Get the colour of the given pixel in the cell array
		Color GetViewportPixel( int iX, int iY )
		{
			if (iX < 0 || iY < 0 || iX >= m_kiCellArrayWidth || iY >= m_kiCellArrayHeight)
			{
				return Color::Empty;
			}

			return m_aCells[iY, iX];
		}


		// Set the colour of the given pixel in the cell array
		// Requires a call to UpdateCellWindow after all drawing has taken place
		void SetViewportPixel( int iX, int iY, Color colour )
		{
			if (iX < 0 || iY < 0 || iX >= m_kiCellArrayWidth || iY >= m_kiCellArrayHeight)
			{
				return;
			}
	
			// Use second line to remove alpha from viewport pixels
//			m_aCells[iY, iX] = colour;
			m_aCells[ iY,iX ] = Color::FromArgb( colour.R, colour.G, colour.B );
		}


		//////////////////////////////////
		// 2D Geometry Rendering

		// These functions are contained in PixelDrawing.cpp

		// Draw a rectangle with top-left at (X, Y) with given width, height and colour
		void DrawRectangle( int X, int Y, int Width, int Height, Color PixelColour );

		// Draw a line from (X1, Y1) to (X2, Y2) in given colour
		void DrawLine( int X1, int Y1, int X2, int Y2, Color PixelColour );

		// Draw a regular polygon with the given number of sides. The centre of the polygon will be at
		// (X, Y) and the points are on a circle of radius R
		void DrawPolygon( int Sides, int X, int Y, int R, Color PixelColour );

		// Draw a regular star with the given number of points. The centre of the star will be at (X, Y)
		// and it the points are on a circle of radius R. The number of points will always be odd.
		void DrawStar( int Sides, int X, int Y, int R, Color PixelColour );

		// Draw a circle with centre (X, Y) and radiuses RX (horizontal) and RY (vertical)
		void DrawCircle( int X, int Y, int R, Color PixelColour );


		//////////////////////////////////
		// Bitmap/Sprite Rendering

		// These functions are contained in SpriteDrawing.cpp

		// Simple copy of bitmap to background
		void DrawBitmap( int X, int Y, Bitmap^ SrcBitmap );

		// Draw 'cutout' sprite onto background using the alpha channel to determine the cutout
		void DrawSprite( int X, int Y, Bitmap^ SrcBitmap );

		// Blend sprite onto background using the alpha channel 
		void DrawSpriteAlpha( int X, int Y, Bitmap^ SrcBitmap );

		// Special sprite blending techniques
		void DrawSpriteAdd( int X, int Y, Bitmap^ SrcBitmap );
		void DrawSpriteMultiply( int X, int Y, Bitmap^ SrcBitmap );
		void DrawSpriteLuminosity( int X, int Y, Bitmap^ SrcBitmap );


	//////////////////////////////////
	// Private data
	private:

		// Size of the cell array below
		literal int m_kiCellArrayWidth = 512;
		literal int m_kiCellArrayHeight = 512;

		// The array of cells that will be shown in the cell window
		array<Color, 2>^ m_aCells;

		// Bitmap and Graphics objects for displaying the cells
		Bitmap^ m_bmpCellWindow;
		Graphics^ m_gfxCellWindow;
		Graphics^ m_gfxCellWindowTransient;

		// Offset of the top left of the cell window in the cell array (floating point - sub-cell accuracy)
		float m_fCellWindowOffsetX;
		float m_fCellWindowOffsetY;

		// Window coordinates of the last mouse down event
		int m_iWindowDownX;
		int m_iWindowDownY;

		// Cell coordinates of the last mouse down event
		int m_iCellDownX;
		int m_iCellDownY;

		// Number of sides for polygonal tools
		int m_iSides;

		// Bitmap for bitmap/sprite tools
		Bitmap^ m_bmpTool;
	};
}

