#include <Challengers2Kao3_alpha/SoundExporter.h>
#include <Challengers2Kao3_alpha/WinApi.h>

/* Jakiś randomizer */
#include <ctime>

/* Maksymalna ilość żądanych floatów w buforze sampli */
#define GRANICA_FLOAT_BUF 512


////////////////////////////////////////////////////////////////
// Zakoduj i zapisz dane z użyciem biblioteki "libvorbis_static"
// "Ogg Vorbis: Encode"
////////////////////////////////////////////////////////////////
int CSoundExporter::encodeWithVorbis()
{
	/* Akceptujemy tylko 16-bitowe próbki */
	if (sizeof(int16_t) != (Wav.Format.wBitsPerSample / 0x08))
	{
		throw EKomunikat(L"Only 16-bit samples are allowed in Vorbis encoding function!");
		return 1;
	}

	showProgress(0x02, (-1));

	/* take physical pages, weld into a logical stream of packets */
	ogg_stream_state os;

	/* one Ogg bitstream page. Vorbis packets are inside */
	ogg_page og;

	/* one raw packet of data for decode */
	ogg_packet op;

	/* struct that stores all the static vorbis bitstream settings */
	vorbis_info vi;

	/* struct that stores all the user comments */
	vorbis_comment vc;

	/* central working state for the packet->PCM decoder */
	vorbis_dsp_state vd;

	/* local working space for packet->PCM decode */
	vorbis_block vb;

	/* Pozostałe wartości */
	int32_t eos = 0;
	const int32_t basic_sample_size = sizeof(int16_t);
	const int32_t data_sample_size = (Wav.Format.wChannels * basic_sample_size);
	int32_t all_data_samples = (Wav.Data.chunkSize / data_sample_size);
	int32_t samples_left;
	byte* data_ptr = Wav.Data.Samples;

	////////////////////////////////////////////////////////////////
	// Encode setup
	////////////////////////////////////////////////////////////////

	vorbis_info_init(&vi);

	/* choose an encoding mode */
	if (0 != vorbis_encode_init_vbr(&vi, Wav.Format.wChannels, Wav.Format.dwSamplesPerSec, 0.4f))
	{
		return 1;	
	}

	/* add a comment */
	vorbis_comment_init(&vc);
	//// (--dsp--) vorbis_comment_add_tag(&vc, "ENCODER", "encoder_example.c");

	/* set up the analysis state and auxiliary encoding storage */
	vorbis_analysis_init(&vd, &vi);
	vorbis_block_init(&vd, &vb);

	/* set up our packet->stream encoder */
	/* pick a random serial number; that way we can more likely build chained streams just by concatenation */
	std::srand((unsigned int)std::time(NULL));
	ogg_stream_init(&os, std::rand());

	/* Vorbis streams begin with three headers; */
	/* the initial header (with most of the codec setup parameters) which is mandated by the Ogg bitstream spec.*/
	/* The second header holds any comment fields. */
	/* The third header holds the bitstream codebook. */
	{
		ogg_packet header;
		ogg_packet header_comm;
		ogg_packet header_code;

		vorbis_analysis_headerout(&vd, &vc, &header, &header_comm, &header_code);
		ogg_stream_packetin(&os, &header);
		ogg_stream_packetin(&os, &header_comm);
		ogg_stream_packetin(&os, &header_code);

		/* This ensures the actual audio data will start on a new page, as per spec */
		while (0 == eos)
		{
			if (0 == ogg_stream_flush(&os, &og))
			{
				break;
			}
			zapiszPlik(og.header, og.header_len);
			zapiszPlik(og.body, og.body_len);
		}
	}

	while (0 == eos)
	{
		/* Wysyłaj bloki po jakieś 512 Floatów, bo były problemy ze stosem */
		/* Każda próbka Stereo ma po 2 kanały. Razem dostajemy 4096 bajtów. */
		samples_left = (all_data_samples > GRANICA_FLOAT_BUF) ? GRANICA_FLOAT_BUF : all_data_samples;

		if (samples_left > 0)
		{
			/* expose the buffer to submit data */
			float** buffer = vorbis_analysis_buffer(&vd, samples_left);

			/* uninterleave samples */
			for (int32_t smpl = 0; smpl < samples_left; smpl++)
			{
				for (int32_t chnl = 0; chnl < Wav.Format.wChannels; chnl++)
				{
					buffer[chnl][smpl] = (*(int16_t*)&(data_ptr[(data_sample_size * smpl) + (basic_sample_size * chnl)])) / 32768.f;
				}
			}

			/* tell the library how much we actually submitted */
			vorbis_analysis_wrote(&vd, samples_left);

			all_data_samples -= GRANICA_FLOAT_BUF;
			data_ptr += (data_sample_size * samples_left);
			if ((data_ptr - Wav.Data.Samples) > Wav.Data.chunkSize)
			{
				throw EKomunikat(L"Data pointer went beyond the scope of input buffer size...");
				return 1;
			}
		}
		else
		{
			all_data_samples = 0;

			/* Tell the library we're at end of stream so that it can handle the last frame and mark end of stream in the output properly */
			vorbis_analysis_wrote(&vd, 0);
		}

		/* vorbis does some data preanalysis, then divvies up blocks for more involved (potentially parallel) processing. */
		/* Get a single block for encoding now */
		while (1 == vorbis_analysis_blockout(&vd, &vb))
		{
			/* analysis, assume we want to use bitrate management */
			vorbis_analysis(&vb, NULL);
			vorbis_bitrate_addblock(&vb);

			while (0 != vorbis_bitrate_flushpacket(&vd, &op))
			{
				/* weld the packet into the bitstream */
				ogg_stream_packetin(&os, &op);

				/* write out pages (if any) */
				while (0 == eos)
				{
					if (0 == ogg_stream_pageout(&os, &og))
					{
						break;
					}
					zapiszPlik(og.header, og.header_len);
					zapiszPlik(og.body, og.body_len);

					/* vorbis knows where the stream ends */
					if (0 != ogg_page_eos(&og))
					{
						eos = 1;
					}
				}
			}
		}

		showProgress(0x02, (Wav.Data.chunkSize - (data_sample_size * all_data_samples)) / float(Wav.Data.chunkSize));
	}

	/* clean up and exit */
	CWindow::zmienTekstOknaLadowania(L"");

	ogg_stream_clear(&os);
	vorbis_block_clear(&vb);
	vorbis_dsp_clear(&vd);
	vorbis_comment_clear(&vc);
	vorbis_info_clear(&vi);

	return 0;
}
