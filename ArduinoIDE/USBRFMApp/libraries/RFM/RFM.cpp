#include <RFM.h>

RFM::RFM(Node* parent, const char* name) : channel(this), Node(parent, name) {

}

RFM::~RFM() {

}

void RFM::setup() {
	this->applySettings();
}

void RFM::loop() {
	this->channel.loop();
}

void RFM::applySettings() {
	// This call is optional, it is needed only if you have to change the default SPI interface used
	// LoRa.setSPI(spi);

	CDS::Iterator storage = this->storage();

	CDS::Iterator pinsStorage = CDS::Object::taketo(storage, K("pins"));
	CDS::Iterator nssStorage = CDS::Object::taketo(pinsStorage, K("nss"));
	CDS::DataBuffer* savedNss = CDS::Element::nextElement(&nssStorage);
	uint8_t nss = CDS::Number::value<uint8_t>(savedNss);
	delete savedNss;
	CDS::Iterator rstStorage = CDS::Object::taketo(pinsStorage, K("rst"));
	CDS::DataBuffer* savedRst = CDS::Element::nextElement(&rstStorage);
	uint8_t rst = CDS::Number::value<uint8_t>(savedRst);
	delete savedRst;
	CDS::Iterator dioStorage = CDS::Object::taketo(pinsStorage, K("dio"));
	CDS::Iterator dio0Storage = CDS::Array::taketo(dioStorage, 0);
	CDS::DataBuffer* savedDio0 = CDS::Element::nextElement(&dio0Storage);
	uint8_t dio0 = CDS::Number::value<uint8_t>(savedDio0);
	delete savedDio0;
	LoRa.setPins(nss, rst, dio0);

	CDS::Iterator freqStorage = CDS::Object::taketo(storage, K("freq"));
	CDS::Iterator currStorage = CDS::Object::taketo(freqStorage, K("curr"));
	CDS::DataBuffer* savedCurr = CDS::Element::nextElement(&currStorage);
	uint32_t curr = CDS::Number::value<uint32_t>(savedCurr);
	delete savedCurr;
	this->active = LoRa.begin(curr);

	if (this->active) {
		CDS::Iterator txpwStorage = CDS::Object::taketo(storage, K("txpw"));
		CDS::DataBuffer* savedTxpw = CDS::Element::nextElement(&txpwStorage);
		uint8_t txpw = CDS::Number::value<uint8_t>(savedTxpw);
		delete savedTxpw;
		LoRa.setTxPower(txpw);
		
		CDS::Iterator sfacStorage = CDS::Object::taketo(storage, K("sfac"));
		CDS::DataBuffer* savedSfac = CDS::Element::nextElement(&sfacStorage);
		uint8_t sfac = CDS::Number::value<uint8_t>(savedSfac);
		delete savedSfac;
		LoRa.setSpreadingFactor(sfac);

		CDS::Iterator sbwStorage = CDS::Object::taketo(storage, K("sbw"));
		CDS::DataBuffer* savedSbw = CDS::Element::nextElement(&sbwStorage);
		uint32_t sbw = CDS::Number::value<uint32_t>(savedSbw);
		delete savedSbw;
		LoRa.setSignalBandwidth(sbw);

		CDS::Iterator cratStorage = CDS::Object::taketo(storage, K("crat"));
		CDS::DataBuffer* savedCrat = CDS::Element::nextElement(&cratStorage);
		uint8_t crat = CDS::Number::value<uint8_t>(savedCrat);
		delete savedCrat;
		LoRa.setCodingRate4(crat);

		CDS::Iterator plengthStorage = CDS::Object::taketo(storage, K("plength"));
		CDS::DataBuffer* savedPlength = CDS::Element::nextElement(&plengthStorage);
		uint8_t plength = CDS::Number::value<uint8_t>(savedPlength);
		delete savedPlength;
		LoRa.setPreambleLength(plength);

		CDS::Iterator swStorage = CDS::Object::taketo(storage, K("sw"));
		CDS::DataBuffer* savedSw = CDS::Element::nextElement(&swStorage);
		uint8_t sw = CDS::Number::value<uint8_t>(savedSw);
		delete savedSw;
		LoRa.setSyncWord(sw);
		
		CDS::Iterator crcStorage = CDS::Object::taketo(storage, K("crc"));
		CDS::DataBuffer* savedCrc = CDS::Element::nextElement(&crcStorage);
		uint8_t crc = CDS::Number::value<uint8_t>(savedCrc);
		delete savedCrc;
		if (crc) {
			LoRa.enableCrc();
		} else {
			LoRa.disableCrc();
		}

		CDS::Iterator iiqStorage = CDS::Object::taketo(storage, K("iiq"));
		CDS::DataBuffer* savedIiq = CDS::Element::nextElement(&iiqStorage);
		uint8_t iiq = CDS::Number::value<uint8_t>(savedIiq);
		delete savedIiq;
		if (iiq) {
			LoRa.enableInvertIQ();
		} else {
			LoRa.disableInvertIQ();
		}
	}
	
	yield();
}

void RFM::state(CDS::DataBuffer* params, CDS::DataBuffer* response) {
	CDS::DataBuffer* object = this->rootIT(response);
	CDS::DataBuffer* state = CDS::Object::newObject(object, K("state"));
	
	CDS::DataBuffer* status = CDS::Object::newNumber(state, K("status"));
	CDS::Number::set(status, this->active);
	
	CDS::Iterator storage = this->storage();
	CDS::DataBuffer* config = CDS::Element::nextElement(&storage);
	CDS::Object::set(state, K("config"), config);
}

void RFM::from(CDS::DataBuffer* params) {
	CDS::Iterator storage = this->storage();
	CDS::Element::fill(storage, params);
	#ifdef ESP8266
		EEPROM.commit();
	#endif
}

  

