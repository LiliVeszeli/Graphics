#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "ColourConversions.h"

namespace PixelPlotter {

	/// <summary>
	/// Summary for ColourSpaceForm
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class ColourSpaceForm : public System::Windows::Forms::Form
	{
	public:
		ColourSpaceForm(void)
		{
			InitializeComponent();

			trackBarRed->Value = 0;
			trackBarGreen->Value = 0;
			trackBarBlue->Value = 0;
			CalculateHSL();
			pictureBoxColour->BackColor = 
				Color::FromArgb( trackBarRed->Value, trackBarGreen->Value, trackBarBlue->Value );
		}

		ColourSpaceForm( int iInitR, int iInitG, int iInitB )
		{
			InitializeComponent();

			trackBarRed->Value = iInitR;
			trackBarGreen->Value = iInitG;
			trackBarBlue->Value = iInitB;
			CalculateHSL();
			pictureBoxColour->BackColor = 
				Color::FromArgb( trackBarRed->Value, trackBarGreen->Value, trackBarBlue->Value );
		}

		Color GetColour()
		{
			return Color::FromArgb( trackBarRed->Value, trackBarGreen->Value, trackBarBlue->Value );
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~ColourSpaceForm()
		{
			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::TrackBar^  trackBarRed;
	private: System::Windows::Forms::Button^  btnSelect;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TextBox^  textBoxRed;
	private: System::Windows::Forms::TextBox^  textBoxGreen;
	private: System::Windows::Forms::TextBox^  textBoxBlue;
	private: System::Windows::Forms::TextBox^  textBoxHue;
	private: System::Windows::Forms::TextBox^  textBoxSat;
	private: System::Windows::Forms::TextBox^  textBoxLight;
	private: System::Windows::Forms::PictureBox^  pictureBoxColour;
	private: System::Windows::Forms::TrackBar^  trackBarGreen;
	private: System::Windows::Forms::TrackBar^  trackBarBlue;
	private: System::Windows::Forms::TrackBar^  trackBarHue;
	private: System::Windows::Forms::TrackBar^  trackBarSat;
	private: System::Windows::Forms::TrackBar^  trackBarLight;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::Button^  btnCancel;


	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->trackBarRed = (gcnew System::Windows::Forms::TrackBar());
			this->btnSelect = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->textBoxRed = (gcnew System::Windows::Forms::TextBox());
			this->textBoxGreen = (gcnew System::Windows::Forms::TextBox());
			this->textBoxBlue = (gcnew System::Windows::Forms::TextBox());
			this->textBoxHue = (gcnew System::Windows::Forms::TextBox());
			this->textBoxSat = (gcnew System::Windows::Forms::TextBox());
			this->textBoxLight = (gcnew System::Windows::Forms::TextBox());
			this->pictureBoxColour = (gcnew System::Windows::Forms::PictureBox());
			this->trackBarGreen = (gcnew System::Windows::Forms::TrackBar());
			this->trackBarBlue = (gcnew System::Windows::Forms::TrackBar());
			this->trackBarHue = (gcnew System::Windows::Forms::TrackBar());
			this->trackBarSat = (gcnew System::Windows::Forms::TrackBar());
			this->trackBarLight = (gcnew System::Windows::Forms::TrackBar());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->btnCancel = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarRed))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBoxColour))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarGreen))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarBlue))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarHue))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarSat))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarLight))->BeginInit();
			this->SuspendLayout();
			// 
			// trackBarRed
			// 
			this->trackBarRed->AutoSize = false;
			this->trackBarRed->Location = System::Drawing::Point(229, 20);
			this->trackBarRed->Maximum = 255;
			this->trackBarRed->Name = L"trackBarRed";
			this->trackBarRed->Size = System::Drawing::Size(128, 18);
			this->trackBarRed->TabIndex = 12;
			this->trackBarRed->TickStyle = System::Windows::Forms::TickStyle::None;
			this->trackBarRed->ValueChanged += gcnew System::EventHandler(this, &ColourSpaceForm::trackBarRed_ValueChanged);
			// 
			// btnSelect
			// 
			this->btnSelect->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->btnSelect->Location = System::Drawing::Point(277, 191);
			this->btnSelect->Name = L"btnSelect";
			this->btnSelect->Size = System::Drawing::Size(80, 23);
			this->btnSelect->TabIndex = 18;
			this->btnSelect->Text = L"Select";
			this->btnSelect->Click += gcnew System::EventHandler(this, &ColourSpaceForm::btnSelect_Click);
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(178, 23);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(16, 13);
			this->label1->TabIndex = 0;
			this->label1->Text = L"R";
			// 
			// textBoxRed
			// 
			this->textBoxRed->Location = System::Drawing::Point(196, 20);
			this->textBoxRed->Name = L"textBoxRed";
			this->textBoxRed->Size = System::Drawing::Size(24, 20);
			this->textBoxRed->TabIndex = 1;
			this->textBoxRed->Text = L"0";
			this->textBoxRed->Leave += gcnew System::EventHandler(this, &ColourSpaceForm::textBoxRed_Leave);
			// 
			// textBoxGreen
			// 
			this->textBoxGreen->Location = System::Drawing::Point(196, 44);
			this->textBoxGreen->Name = L"textBoxGreen";
			this->textBoxGreen->Size = System::Drawing::Size(24, 20);
			this->textBoxGreen->TabIndex = 3;
			this->textBoxGreen->Text = L"0";
			this->textBoxGreen->Leave += gcnew System::EventHandler(this, &ColourSpaceForm::textBoxGreen_Leave);
			// 
			// textBoxBlue
			// 
			this->textBoxBlue->Location = System::Drawing::Point(196, 68);
			this->textBoxBlue->Name = L"textBoxBlue";
			this->textBoxBlue->Size = System::Drawing::Size(24, 20);
			this->textBoxBlue->TabIndex = 5;
			this->textBoxBlue->Text = L"0";
			this->textBoxBlue->Leave += gcnew System::EventHandler(this, &ColourSpaceForm::textBoxBlue_Leave);
			// 
			// textBoxHue
			// 
			this->textBoxHue->Location = System::Drawing::Point(196, 108);
			this->textBoxHue->Name = L"textBoxHue";
			this->textBoxHue->Size = System::Drawing::Size(24, 20);
			this->textBoxHue->TabIndex = 7;
			this->textBoxHue->Text = L"0";
			this->textBoxHue->Leave += gcnew System::EventHandler(this, &ColourSpaceForm::textBoxHue_Leave);
			// 
			// textBoxSat
			// 
			this->textBoxSat->Location = System::Drawing::Point(196, 132);
			this->textBoxSat->Name = L"textBoxSat";
			this->textBoxSat->Size = System::Drawing::Size(24, 20);
			this->textBoxSat->TabIndex = 9;
			this->textBoxSat->Text = L"0";
			this->textBoxSat->Leave += gcnew System::EventHandler(this, &ColourSpaceForm::textBoxSat_Leave);
			// 
			// textBoxLight
			// 
			this->textBoxLight->Location = System::Drawing::Point(196, 156);
			this->textBoxLight->Name = L"textBoxLight";
			this->textBoxLight->Size = System::Drawing::Size(24, 20);
			this->textBoxLight->TabIndex = 11;
			this->textBoxLight->Text = L"0";
			this->textBoxLight->Leave += gcnew System::EventHandler(this, &ColourSpaceForm::textBoxLight_Leave);
			// 
			// pictureBoxColour
			// 
			this->pictureBoxColour->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left));
			this->pictureBoxColour->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->pictureBoxColour->Location = System::Drawing::Point(12, 12);
			this->pictureBoxColour->Name = L"pictureBoxColour";
			this->pictureBoxColour->Size = System::Drawing::Size(160, 167);
			this->pictureBoxColour->TabIndex = 5;
			this->pictureBoxColour->TabStop = false;
			// 
			// trackBarGreen
			// 
			this->trackBarGreen->AutoSize = false;
			this->trackBarGreen->Location = System::Drawing::Point(228, 44);
			this->trackBarGreen->Maximum = 255;
			this->trackBarGreen->Name = L"trackBarGreen";
			this->trackBarGreen->Size = System::Drawing::Size(128, 18);
			this->trackBarGreen->TabIndex = 13;
			this->trackBarGreen->TickStyle = System::Windows::Forms::TickStyle::None;
			this->trackBarGreen->ValueChanged += gcnew System::EventHandler(this, &ColourSpaceForm::trackBarGreen_ValueChanged);
			// 
			// trackBarBlue
			// 
			this->trackBarBlue->AutoSize = false;
			this->trackBarBlue->Location = System::Drawing::Point(228, 68);
			this->trackBarBlue->Maximum = 255;
			this->trackBarBlue->Name = L"trackBarBlue";
			this->trackBarBlue->Size = System::Drawing::Size(128, 18);
			this->trackBarBlue->TabIndex = 14;
			this->trackBarBlue->TickStyle = System::Windows::Forms::TickStyle::None;
			this->trackBarBlue->ValueChanged += gcnew System::EventHandler(this, &ColourSpaceForm::trackBarBlue_ValueChanged);
			// 
			// trackBarHue
			// 
			this->trackBarHue->AutoSize = false;
			this->trackBarHue->Location = System::Drawing::Point(228, 108);
			this->trackBarHue->Maximum = 359;
			this->trackBarHue->Name = L"trackBarHue";
			this->trackBarHue->Size = System::Drawing::Size(128, 18);
			this->trackBarHue->TabIndex = 15;
			this->trackBarHue->TickStyle = System::Windows::Forms::TickStyle::None;
			this->trackBarHue->ValueChanged += gcnew System::EventHandler(this, &ColourSpaceForm::trackBarHue_ValueChanged);
			// 
			// trackBarSat
			// 
			this->trackBarSat->AutoSize = false;
			this->trackBarSat->Location = System::Drawing::Point(228, 132);
			this->trackBarSat->Maximum = 100;
			this->trackBarSat->Name = L"trackBarSat";
			this->trackBarSat->Size = System::Drawing::Size(128, 18);
			this->trackBarSat->TabIndex = 16;
			this->trackBarSat->TickStyle = System::Windows::Forms::TickStyle::None;
			this->trackBarSat->ValueChanged += gcnew System::EventHandler(this, &ColourSpaceForm::trackBarSat_ValueChanged);
			// 
			// trackBarLight
			// 
			this->trackBarLight->AutoSize = false;
			this->trackBarLight->Location = System::Drawing::Point(228, 156);
			this->trackBarLight->Maximum = 100;
			this->trackBarLight->Name = L"trackBarLight";
			this->trackBarLight->Size = System::Drawing::Size(128, 18);
			this->trackBarLight->TabIndex = 17;
			this->trackBarLight->TickStyle = System::Windows::Forms::TickStyle::None;
			this->trackBarLight->ValueChanged += gcnew System::EventHandler(this, &ColourSpaceForm::trackBarLight_ValueChanged);
			// 
			// label3
			// 
			this->label3->Location = System::Drawing::Point(178, 47);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(16, 13);
			this->label3->TabIndex = 2;
			this->label3->Text = L"G";
			// 
			// label4
			// 
			this->label4->Location = System::Drawing::Point(178, 71);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(16, 13);
			this->label4->TabIndex = 4;
			this->label4->Text = L"B";
			// 
			// label5
			// 
			this->label5->Location = System::Drawing::Point(178, 111);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(16, 13);
			this->label5->TabIndex = 6;
			this->label5->Text = L"H";
			// 
			// label6
			// 
			this->label6->Location = System::Drawing::Point(178, 135);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(16, 13);
			this->label6->TabIndex = 8;
			this->label6->Text = L"S";
			// 
			// label7
			// 
			this->label7->Location = System::Drawing::Point(178, 159);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(16, 13);
			this->label7->TabIndex = 10;
			this->label7->Text = L"L";
			// 
			// btnCancel
			// 
			this->btnCancel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->btnCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->btnCancel->Location = System::Drawing::Point(191, 191);
			this->btnCancel->Name = L"btnCancel";
			this->btnCancel->Size = System::Drawing::Size(80, 23);
			this->btnCancel->TabIndex = 18;
			this->btnCancel->Text = L"Cancel";
			this->btnCancel->Click += gcnew System::EventHandler(this, &ColourSpaceForm::btnCancel_Click);
			// 
			// ColourSpaceForm
			// 
			this->AcceptButton = this->btnSelect;
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->CancelButton = this->btnCancel;
			this->ClientSize = System::Drawing::Size(368, 226);
			this->Controls->Add(this->trackBarRed);
			this->Controls->Add(this->btnCancel);
			this->Controls->Add(this->btnSelect);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->textBoxRed);
			this->Controls->Add(this->textBoxGreen);
			this->Controls->Add(this->textBoxBlue);
			this->Controls->Add(this->textBoxHue);
			this->Controls->Add(this->textBoxSat);
			this->Controls->Add(this->textBoxLight);
			this->Controls->Add(this->pictureBoxColour);
			this->Controls->Add(this->trackBarGreen);
			this->Controls->Add(this->trackBarBlue);
			this->Controls->Add(this->trackBarHue);
			this->Controls->Add(this->trackBarSat);
			this->Controls->Add(this->trackBarLight);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->label7);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->Name = L"ColourSpaceForm";
			this->Text = L"Colour Space Picker";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarRed))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBoxColour))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarGreen))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarBlue))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarHue))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarSat))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarLight))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

		// Edit boxes
		System::Void textBoxRed_Leave( System::Object^ sender, System::EventArgs^ e )
		{
			int iNewRed;
			try
			{
				iNewRed = Convert::ToInt32( textBoxRed->Text );
			}
			catch (Exception^) 
			{
				textBoxRed->Text = "0";
				iNewRed = 0;
			}
			if (iNewRed == trackBarRed->Value)
			{
				return;
			}
			if (iNewRed > 255)
			{
				iNewRed = 255;
				textBoxRed->Text = "255";
			}
			if (iNewRed < 0)
			{
				iNewRed = 0;
				textBoxRed->Text = "0";
			}
			trackBarRed->Value = iNewRed;

			CalculateHSL();
			pictureBoxColour->BackColor = 
				Color::FromArgb( trackBarRed->Value, trackBarGreen->Value, trackBarBlue->Value );
		}

		System::Void textBoxGreen_Leave( System::Object^ sender, System::EventArgs^ e )
		{
			int iNewGreen;
			try
			{
				iNewGreen = Convert::ToInt32( textBoxGreen->Text );
			}
			catch (Exception^) 
			{
				textBoxGreen->Text = "0";
				iNewGreen = 0;
			}
			if (iNewGreen == trackBarGreen->Value)
			{
				return;
			}
			if (iNewGreen > 255)
			{
				iNewGreen = 255;
				textBoxGreen->Text = "255";
			}
			if (iNewGreen < 0)
			{
				iNewGreen = 0;
				textBoxGreen->Text = "0";
			}
			trackBarGreen->Value = iNewGreen;

			CalculateHSL();
			pictureBoxColour->BackColor = 
				Color::FromArgb( trackBarRed->Value, trackBarGreen->Value, trackBarBlue->Value );
		}

		System::Void textBoxBlue_Leave( System::Object^ sender, System::EventArgs^ e )
		{
			int iNewBlue;
			try
			{
				iNewBlue = Convert::ToInt32( textBoxBlue->Text );
			}
			catch (Exception^) 
			{
				textBoxBlue->Text = "0";
				iNewBlue = 0;
			}
			if (iNewBlue == trackBarBlue->Value)
			{
				return;
			}
			if (iNewBlue > 255)
			{
				iNewBlue = 255;
				textBoxBlue->Text = "255";
			}
			if (iNewBlue < 0)
			{
				iNewBlue = 0;
				textBoxBlue->Text = "0";
			}
			trackBarBlue->Value = iNewBlue;

			CalculateHSL();
			pictureBoxColour->BackColor = 
				Color::FromArgb( trackBarRed->Value, trackBarGreen->Value, trackBarBlue->Value );
		}

		System::Void textBoxHue_Leave( System::Object^ sender, System::EventArgs^ e )
		{
			int iNewHue;
			try
			{
				iNewHue = Convert::ToInt32( textBoxHue->Text );
			}
			catch (Exception^) 
			{
				textBoxHue->Text = "0";
				iNewHue = 0;
			}
			if (iNewHue == trackBarHue->Value)
			{
				return;
			}
			if (iNewHue > 359)
			{
				iNewHue = 359;
				textBoxHue->Text = "359";
			}
			if (iNewHue < 0)
			{
				iNewHue = 0;
				textBoxHue->Text = "0";
			}
			trackBarHue->Value = iNewHue;

			CalculateRGB();
			pictureBoxColour->BackColor = 
				Color::FromArgb( trackBarRed->Value, trackBarGreen->Value, trackBarBlue->Value );
		}

		System::Void textBoxSat_Leave( System::Object^ sender, System::EventArgs^ e )
		{
			int iNewSat;
			try
			{
				iNewSat = Convert::ToInt32( textBoxSat->Text );
			}
			catch (Exception^) 
			{
				textBoxSat->Text = "0";
				iNewSat = 0;
			}
			if (iNewSat == trackBarSat->Value)
			{
				return;
			}
			if (iNewSat > 100)
			{
				iNewSat = 100;
				textBoxSat->Text = "100";
			}
			if (iNewSat < 0)
			{
				iNewSat = 0;
				textBoxSat->Text = "0";
			}
			trackBarSat->Value = iNewSat;

			CalculateRGB();
			pictureBoxColour->BackColor = 
				Color::FromArgb( trackBarRed->Value, trackBarGreen->Value, trackBarBlue->Value );
		}

		System::Void textBoxLight_Leave( System::Object^ sender, System::EventArgs^ e )
		{
			int iNewLight;
			try
			{
				iNewLight = Convert::ToInt32( textBoxLight->Text );
			}
			catch (Exception^) 
			{
				textBoxLight->Text = "0";
				iNewLight = 0;
			}
			if (iNewLight == trackBarLight->Value)
			{
				return;
			}
			if (iNewLight > 100)
			{
				iNewLight = 100;
				textBoxLight->Text = "100";
			}
			if (iNewLight < 0)
			{
				iNewLight = 0;
				textBoxLight->Text = "0";
			}
			trackBarLight->Value = iNewLight;

			CalculateRGB();
			pictureBoxColour->BackColor = 
				Color::FromArgb( trackBarRed->Value, trackBarGreen->Value, trackBarBlue->Value );
		}


		// Track bar controls
		System::Void trackBarRed_ValueChanged( System::Object^ sender, System::EventArgs^ e )
		{
			if (trackbarUpdate) return; // Prevent reentrancy when automatically updating other trackbars
			textBoxRed->Text = Convert::ToString( trackBarRed->Value );
			CalculateHSL();
			pictureBoxColour->BackColor = 
				Color::FromArgb( trackBarRed->Value, trackBarGreen->Value, trackBarBlue->Value );
		}

		System::Void trackBarGreen_ValueChanged( System::Object^ sender, System::EventArgs^ e )
		{
			if (trackbarUpdate) return; // Prevent reentrancy when automatically updating other trackbars
			textBoxGreen->Text = Convert::ToString( trackBarGreen->Value );
			CalculateHSL();
			pictureBoxColour->BackColor = 
				Color::FromArgb( trackBarRed->Value, trackBarGreen->Value, trackBarBlue->Value );
		}

		System::Void trackBarBlue_ValueChanged( System::Object^ sender, System::EventArgs^ e )
		{
			if (trackbarUpdate) return; // Prevent reentrancy when automatically updating other trackbars
			textBoxBlue->Text = Convert::ToString( trackBarBlue->Value );
			CalculateHSL();
			pictureBoxColour->BackColor = 
				Color::FromArgb( trackBarRed->Value, trackBarGreen->Value, trackBarBlue->Value );
		}

		System::Void trackBarHue_ValueChanged( System::Object^ sender, System::EventArgs^ e )
		{
			if (trackbarUpdate) return; // Prevent reentrancy when automatically updating other trackbars
			textBoxHue->Text = Convert::ToString( trackBarHue->Value );
			CalculateRGB();
			pictureBoxColour->BackColor = 
				Color::FromArgb( trackBarRed->Value, trackBarGreen->Value, trackBarBlue->Value );
		}

		System::Void trackBarSat_ValueChanged( System::Object^ sender, System::EventArgs^ e )
		{
			if (trackbarUpdate) return; // Prevent reentrancy when automatically updating other trackbars
			textBoxSat->Text = Convert::ToString( trackBarSat->Value );
			CalculateRGB();
			pictureBoxColour->BackColor = 
				Color::FromArgb( trackBarRed->Value, trackBarGreen->Value, trackBarBlue->Value );
		}

		System::Void trackBarLight_ValueChanged( System::Object^ sender, System::EventArgs^ e )
		{
			if (trackbarUpdate) return; // Prevent reentrancy when automatically updating other trackbars
			textBoxLight->Text = Convert::ToString( trackBarLight->Value );
			CalculateRGB();
			pictureBoxColour->BackColor = 
				Color::FromArgb( trackBarRed->Value, trackBarGreen->Value, trackBarBlue->Value );
		}


		// Select/Cancel buttons
		System::Void btnSelect_Click( System::Object^ sender, System::EventArgs^ e )
		{
			DialogResult = ::DialogResult::OK;
			Close();
		}
		System::Void btnCancel_Click(System::Object^ sender, System::EventArgs^ e)
		{
			DialogResult = ::DialogResult::Cancel;
			Close();
		}


		// Calculate the HSL values from the RGB values
		void CalculateHSL()
		{
			// Handle managed variables
			int iH = trackBarHue->Value;
			int iS = trackBarSat->Value;
			int iL = trackBarLight->Value;

			// Call colour space conversion function to set new HSL values
			RGBToHSL( trackBarRed->Value, trackBarGreen->Value, trackBarBlue->Value, iH, iS, iL );

			// Update other components - prevent reentrancy
			trackbarUpdate = true;
			trackBarHue->Value = iH;
			trackBarSat->Value = iS;
			trackBarLight->Value = iL;
			textBoxHue->Text = Convert::ToString( iH );
			textBoxSat->Text = Convert::ToString( iS );
			textBoxLight->Text = Convert::ToString( iL );
			trackbarUpdate = false;
		}


		// Calculate the RGB values from the HSL values
		void CalculateRGB()
		{
			// Handle managed variables
			int iR = trackBarRed->Value;
			int iG = trackBarGreen->Value;
			int iB = trackBarBlue->Value;
			
			// Call colour space conversion function
			HSLToRGB( trackBarHue->Value, trackBarSat->Value, trackBarLight->Value, iR, iG, iB );
			
			// Update other components - prevent reentrancy
			trackbarUpdate = true;
			trackBarRed->Value = iR;
			trackBarGreen->Value = iG;
			trackBarBlue->Value = iB;
			textBoxRed->Text = Convert::ToString( iR );
			textBoxGreen->Text = Convert::ToString( iG );
			textBoxBlue->Text = Convert::ToString( iB );
			trackbarUpdate = false;
		}


		// Flag to indicate if trackbar change is internal (rather than direct user control) - used to prevent reentrancy
		bool trackbarUpdate;
	};
}
