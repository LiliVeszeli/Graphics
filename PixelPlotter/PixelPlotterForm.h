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

namespace PixelPlotter {

	/// <summary>
	/// Summary for PixelPlotterForm
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class PixelPlotterForm : public System::Windows::Forms::Form
	{
	public:
		PixelPlotterForm(void)
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

			bool bShowTools = (tbbPolygon->Pushed || tbbStar->Pushed);
			gbxToolSettings->Visible = bShowTools;
			lblSides->Visible = bShowTools;
			tbxSides->Visible = bShowTools;
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~PixelPlotterForm()
		{
			// Release resources created in the constructor
			delete m_gfxCellWindowTransient;
			delete m_gfxCellWindow;

			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::Button^  btnClear;
	private: System::Windows::Forms::StatusBar^  sbMain;
	private: System::Windows::Forms::StatusBarPanel^  statusBarPanel1;
	private: System::Windows::Forms::StatusBarPanel^  sbpXPos;
	private: System::Windows::Forms::StatusBarPanel^  statusBarPanel2;
	private: System::Windows::Forms::StatusBarPanel^  sbpYPos;
	private: System::Windows::Forms::StatusBarPanel^  statusBarPanel3;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TrackBar^  trkPixelSize;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::CheckBox^  cbxGrid;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::PictureBox^  pbxFGColour;
	private: System::Windows::Forms::PictureBox^  pbxBGColour;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::TextBox^  tbxSides;
	private: System::Windows::Forms::PictureBox^  pbxCellWindow;
	private: System::Windows::Forms::ImageList^  ilToolBar;
	private: System::Windows::Forms::GroupBox^  gbxToolSettings;
	private: System::Windows::Forms::Label^  lblSides;
	private: System::Windows::Forms::ToolBarButton^  tbbCircle;
	private: System::Windows::Forms::ToolBar^  tbMain;
	private: System::Windows::Forms::ToolBarButton^  tbbDrag;
	private: System::Windows::Forms::ToolBarButton^  tbbPoint;
	private: System::Windows::Forms::ToolBarButton^  tbbFreehand;
	private: System::Windows::Forms::ToolBarButton^  tbbLine;
	private: System::Windows::Forms::ToolBarButton^  tbbRectangle;
	private: System::Windows::Forms::ToolBarButton^  tbbPolygon;
	private: System::Windows::Forms::ToolBarButton^  tbbStar;
	private: System::ComponentModel::IContainer^  components;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(PixelPlotterForm::typeid));
			this->btnClear = (gcnew System::Windows::Forms::Button());
			this->sbMain = (gcnew System::Windows::Forms::StatusBar());
			this->statusBarPanel1 = (gcnew System::Windows::Forms::StatusBarPanel());
			this->sbpXPos = (gcnew System::Windows::Forms::StatusBarPanel());
			this->statusBarPanel2 = (gcnew System::Windows::Forms::StatusBarPanel());
			this->sbpYPos = (gcnew System::Windows::Forms::StatusBarPanel());
			this->statusBarPanel3 = (gcnew System::Windows::Forms::StatusBarPanel());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->trkPixelSize = (gcnew System::Windows::Forms::TrackBar());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->cbxGrid = (gcnew System::Windows::Forms::CheckBox());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->pbxFGColour = (gcnew System::Windows::Forms::PictureBox());
			this->pbxBGColour = (gcnew System::Windows::Forms::PictureBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->tbxSides = (gcnew System::Windows::Forms::TextBox());
			this->pbxCellWindow = (gcnew System::Windows::Forms::PictureBox());
			this->ilToolBar = (gcnew System::Windows::Forms::ImageList(this->components));
			this->gbxToolSettings = (gcnew System::Windows::Forms::GroupBox());
			this->lblSides = (gcnew System::Windows::Forms::Label());
			this->tbbCircle = (gcnew System::Windows::Forms::ToolBarButton());
			this->tbMain = (gcnew System::Windows::Forms::ToolBar());
			this->tbbDrag = (gcnew System::Windows::Forms::ToolBarButton());
			this->tbbPoint = (gcnew System::Windows::Forms::ToolBarButton());
			this->tbbFreehand = (gcnew System::Windows::Forms::ToolBarButton());
			this->tbbLine = (gcnew System::Windows::Forms::ToolBarButton());
			this->tbbRectangle = (gcnew System::Windows::Forms::ToolBarButton());
			this->tbbPolygon = (gcnew System::Windows::Forms::ToolBarButton());
			this->tbbStar = (gcnew System::Windows::Forms::ToolBarButton());
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
			this->SuspendLayout();
			// 
			// btnClear
			// 
			this->btnClear->Location = System::Drawing::Point(8, 24);
			this->btnClear->Name = L"btnClear";
			this->btnClear->Size = System::Drawing::Size(104, 23);
			this->btnClear->TabIndex = 0;
			this->btnClear->Text = L"Clear";
			this->btnClear->Click += gcnew System::EventHandler(this, &PixelPlotterForm::btnClear_Click);
			// 
			// sbMain
			// 
			this->sbMain->Location = System::Drawing::Point(0, 522);
			this->sbMain->Name = L"sbMain";
			this->sbMain->Panels->AddRange(gcnew cli::array< System::Windows::Forms::StatusBarPanel^  >(5) {this->statusBarPanel1, this->sbpXPos, 
				this->statusBarPanel2, this->sbpYPos, this->statusBarPanel3});
			this->sbMain->ShowPanels = true;
			this->sbMain->Size = System::Drawing::Size(614, 22);
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
			this->statusBarPanel3->Width = 504;
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(8, 56);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(104, 16);
			this->label1->TabIndex = 1;
			this->label1->Text = L"Pixel Size";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// trkPixelSize
			// 
			this->trkPixelSize->AutoSize = false;
			this->trkPixelSize->Location = System::Drawing::Point(8, 72);
			this->trkPixelSize->Maximum = 25;
			this->trkPixelSize->Minimum = 1;
			this->trkPixelSize->Name = L"trkPixelSize";
			this->trkPixelSize->Size = System::Drawing::Size(104, 32);
			this->trkPixelSize->TabIndex = 2;
			this->trkPixelSize->TickFrequency = 4;
			this->trkPixelSize->TickStyle = System::Windows::Forms::TickStyle::TopLeft;
			this->trkPixelSize->Value = 12;
			this->trkPixelSize->Scroll += gcnew System::EventHandler(this, &PixelPlotterForm::trkPixelSize_Scroll);
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->btnClear);
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Controls->Add(this->trkPixelSize);
			this->groupBox1->Controls->Add(this->cbxGrid);
			this->groupBox1->Location = System::Drawing::Point(488, 48);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(120, 144);
			this->groupBox1->TabIndex = 1;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Drawing Area";
			// 
			// cbxGrid
			// 
			this->cbxGrid->Checked = true;
			this->cbxGrid->CheckState = System::Windows::Forms::CheckState::Checked;
			this->cbxGrid->Location = System::Drawing::Point(24, 112);
			this->cbxGrid->Name = L"cbxGrid";
			this->cbxGrid->Size = System::Drawing::Size(77, 24);
			this->cbxGrid->TabIndex = 3;
			this->cbxGrid->Text = L"Show Grid";
			this->cbxGrid->CheckedChanged += gcnew System::EventHandler(this, &PixelPlotterForm::cbxGrid_CheckedChanged);
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->pbxFGColour);
			this->groupBox2->Controls->Add(this->pbxBGColour);
			this->groupBox2->Controls->Add(this->label3);
			this->groupBox2->Controls->Add(this->label2);
			this->groupBox2->Location = System::Drawing::Point(488, 208);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(120, 120);
			this->groupBox2->TabIndex = 2;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Colours";
			// 
			// pbxFGColour
			// 
			this->pbxFGColour->BackColor = System::Drawing::Color::Black;
			this->pbxFGColour->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->pbxFGColour->Location = System::Drawing::Point(80, 24);
			this->pbxFGColour->Name = L"pbxFGColour";
			this->pbxFGColour->Size = System::Drawing::Size(32, 32);
			this->pbxFGColour->TabIndex = 0;
			this->pbxFGColour->TabStop = false;
			this->pbxFGColour->Click += gcnew System::EventHandler(this, &PixelPlotterForm::pbxFGColour_Click);
			// 
			// pbxBGColour
			// 
			this->pbxBGColour->BackColor = System::Drawing::Color::White;
			this->pbxBGColour->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->pbxBGColour->Location = System::Drawing::Point(80, 72);
			this->pbxBGColour->Name = L"pbxBGColour";
			this->pbxBGColour->Size = System::Drawing::Size(32, 32);
			this->pbxBGColour->TabIndex = 0;
			this->pbxBGColour->TabStop = false;
			this->pbxBGColour->Click += gcnew System::EventHandler(this, &PixelPlotterForm::pbxBGColour_Click);
			// 
			// label3
			// 
			this->label3->Location = System::Drawing::Point(8, 80);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(72, 16);
			this->label3->TabIndex = 1;
			this->label3->Text = L"Background";
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(8, 32);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(72, 16);
			this->label2->TabIndex = 0;
			this->label2->Text = L"Foreground";
			// 
			// tbxSides
			// 
			this->tbxSides->Location = System::Drawing::Point(80, 28);
			this->tbxSides->Name = L"tbxSides";
			this->tbxSides->Size = System::Drawing::Size(32, 20);
			this->tbxSides->TabIndex = 1;
			this->tbxSides->Text = L"5";
			this->tbxSides->TextChanged += gcnew System::EventHandler(this, &PixelPlotterForm::tbxSides_TextChanged);
			// 
			// pbxCellWindow
			// 
			this->pbxCellWindow->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->pbxCellWindow->Cursor = System::Windows::Forms::Cursors::Cross;
			this->pbxCellWindow->Location = System::Drawing::Point(2, 40);
			this->pbxCellWindow->Name = L"pbxCellWindow";
			this->pbxCellWindow->Size = System::Drawing::Size(482, 482);
			this->pbxCellWindow->TabIndex = 13;
			this->pbxCellWindow->TabStop = false;
			this->pbxCellWindow->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &PixelPlotterForm::pbxCellWindow_MouseMove);
			this->pbxCellWindow->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &PixelPlotterForm::pbxCellWindow_MouseDown);
			this->pbxCellWindow->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &PixelPlotterForm::pbxCellWindow_MouseUp);
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
			// 
			// gbxToolSettings
			// 
			this->gbxToolSettings->Controls->Add(this->tbxSides);
			this->gbxToolSettings->Controls->Add(this->lblSides);
			this->gbxToolSettings->Location = System::Drawing::Point(488, 344);
			this->gbxToolSettings->Name = L"gbxToolSettings";
			this->gbxToolSettings->Size = System::Drawing::Size(120, 64);
			this->gbxToolSettings->TabIndex = 3;
			this->gbxToolSettings->TabStop = false;
			this->gbxToolSettings->Text = L"Tool Settings";
			// 
			// lblSides
			// 
			this->lblSides->Location = System::Drawing::Point(8, 24);
			this->lblSides->Name = L"lblSides";
			this->lblSides->Size = System::Drawing::Size(64, 32);
			this->lblSides->TabIndex = 0;
			this->lblSides->Text = L"Number of Sides";
			// 
			// tbbCircle
			// 
			this->tbbCircle->ImageIndex = 7;
			this->tbbCircle->Name = L"tbbCircle";
			this->tbbCircle->Style = System::Windows::Forms::ToolBarButtonStyle::ToggleButton;
			this->tbbCircle->ToolTipText = L"Circle Tool";
			// 
			// tbMain
			// 
			this->tbMain->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->tbMain->Buttons->AddRange(gcnew cli::array< System::Windows::Forms::ToolBarButton^  >(8) {this->tbbDrag, this->tbbPoint, 
				this->tbbFreehand, this->tbbLine, this->tbbRectangle, this->tbbPolygon, this->tbbStar, this->tbbCircle});
			this->tbMain->DropDownArrows = true;
			this->tbMain->ImageList = this->ilToolBar;
			this->tbMain->Location = System::Drawing::Point(0, 0);
			this->tbMain->Name = L"tbMain";
			this->tbMain->ShowToolTips = true;
			this->tbMain->Size = System::Drawing::Size(614, 38);
			this->tbMain->TabIndex = 0;
			this->tbMain->ButtonClick += gcnew System::Windows::Forms::ToolBarButtonClickEventHandler(this, &PixelPlotterForm::tbMain_ButtonClick);
			// 
			// tbbDrag
			// 
			this->tbbDrag->ImageIndex = 0;
			this->tbbDrag->Name = L"tbbDrag";
			this->tbbDrag->Style = System::Windows::Forms::ToolBarButtonStyle::ToggleButton;
			this->tbbDrag->ToolTipText = L"Drag Tool";
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
			// PixelPlotterForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(614, 544);
			this->Controls->Add(this->sbMain);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->pbxCellWindow);
			this->Controls->Add(this->gbxToolSettings);
			this->Controls->Add(this->tbMain);
			this->Cursor = System::Windows::Forms::Cursors::Arrow;
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->MaximizeBox = false;
			this->Name = L"PixelPlotterForm";
			this->Text = L"Pixel Plotter";
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
					
					// Interpret initial mouse click based on which tool is in use
					if (tbbPoint->Pushed || tbbFreehand->Pushed)
					{
						SetViewportPixel( m_iCellDownX, m_iCellDownY, colourDraw );
					}
					break;
				}
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
					}
					else if (tbbLine->Pushed || tbbRectangle->Pushed || tbbPolygon->Pushed || 
					         tbbStar->Pushed || tbbCircle->Pushed)
					{
						DrawGuideLines( e->X, e->Y );
					}
					break;
				}
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
						RemoveGuideLines();
						DrawLine( m_iCellDownX, m_iCellDownY, iCellUpX, iCellUpY, colourDraw );
					}
					else if (tbbRectangle->Pushed)
					{
						RemoveGuideLines();
						int iLeftX = Math::Min( m_iCellDownX, iCellUpX );
						int iRightX = Math::Max( m_iCellDownX, iCellUpX );
						int iTopY = Math::Min( m_iCellDownY, iCellUpY );
						int iBottomY = Math::Max( m_iCellDownY, iCellUpY );
						DrawRectangle( iLeftX, iTopY, iRightX - iLeftX + 1, iBottomY - iTopY + 1, colourDraw );
					}
					else if (tbbPolygon->Pushed)
					{
						RemoveGuideLines();
						int iMoveX = iCellUpX - m_iCellDownX;
						int iMoveY = iCellUpY - m_iCellDownY;
						int iRadius = static_cast<int>(Math::Sqrt( iMoveX * iMoveX + iMoveY * iMoveY ) + 0.5f);
						DrawPolygon( m_iSides, m_iCellDownX, m_iCellDownY, iRadius, colourDraw );
					}
					else if (tbbStar->Pushed)
					{
						RemoveGuideLines();
						int iMoveX = iCellUpX - m_iCellDownX;
						int iMoveY = iCellUpY - m_iCellDownY;
						int iRadius = static_cast<int>(Math::Sqrt( iMoveX * iMoveX + iMoveY * iMoveY ) + 0.5f);
						DrawStar( m_iSides, m_iCellDownX, m_iCellDownY, iRadius, colourDraw );
					}
					else if (tbbCircle->Pushed)
					{
						RemoveGuideLines();
						int iMoveX = iCellUpX - m_iCellDownX;
						int iMoveY = iCellUpY - m_iCellDownY;
						int iRadius = static_cast<int>(Math::Sqrt( iMoveX * iMoveX + iMoveY * iMoveY ) + 0.5f);
						DrawCircle( m_iCellDownX, m_iCellDownY, iRadius, colourDraw );
					}
					break;
				}
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
			e->Button->Pushed = true;
			if (e->Button == tbbDrag)
			{
				pbxCellWindow->Cursor = Cursors::Hand;
			}
			else
			{
				pbxCellWindow->Cursor = Cursors::Cross;
			}

			bool bShowTools = (tbbPolygon->Pushed || tbbStar->Pushed);
			gbxToolSettings->Visible = bShowTools;
			lblSides->Visible = bShowTools;
			tbxSides->Visible = bShowTools;
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

		System::Void tbxSides_TextChanged( System::Object^ sender, System::EventArgs^ e )
		{
			try
			{
				m_iSides = Convert::ToInt32( tbxSides->Text );
			}
			catch (Exception^) 
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
		}


		// Remove guide lines upon mouse up event (see DrawGuideLines above)
		void RemoveGuideLines()
		{
			pbxCellWindow->Invalidate();
			pbxCellWindow->Update();
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
			while (iPixY < bmdCellWindow->Height)
			{
				interior_ptr<Color> CurrentCell = &m_aCells[iY, iX];
				int pixelColour = CurrentCell->ToArgb();
				int iCellXPos = iInitCellXPos;
				int iPixX = 0;
				while (iPixX < bmdCellWindow->Width)
				{
					*pPixels = pixelColour;
					++pPixels;
					--iCellXPos;
					if (!iCellXPos)
					{
						iCellXPos = iCellWidth;
						++CurrentCell;
						pixelColour = CurrentCell->ToArgb();
					}

					iPixX++;
				}

				pPixels += iStride;
				--iCellYPos;
				if (!iCellYPos)
				{
					iCellYPos = iCellHeight;
					++iY;
				}

				iPixY++;
			}

			m_bmpCellWindow->UnlockBits( bmdCellWindow );
		}


		// Draw the given pixel in the appropriate place in the cell window
		void SetViewportPixel( int iX, int iY, Color colour )
		{
			if (iX < 0 || iY < 0 || iX >= m_kiCellArrayWidth || iY >= m_kiCellArrayHeight)
			{
				return;
			}

			m_aCells[iY, iX] = colour;
			Brush^ brsPixel = gcnew SolidBrush( colour );
			Rectangle rctPixel;
			if (UseGrid())
			{
				rctPixel = Rectangle(WindowXFromCellX( iX ) + 1, WindowYFromCellY( iY ) + 1, CellWidth() - 1, CellHeight() - 1);
			}
			else
			{
				rctPixel = Rectangle(WindowXFromCellX( iX ), WindowYFromCellY( iY ), CellWidth(), CellHeight());
			}

			m_gfxCellWindow->FillRectangle( brsPixel, rctPixel );
			delete brsPixel;
			pbxCellWindow->Invalidate( rctPixel );
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
		// Private data

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
	};
}
