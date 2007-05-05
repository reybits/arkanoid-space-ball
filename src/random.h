class   CRandom {
protected:
	int	m_nSeed;
public:
	CRandom() {
		m_nSeed	= SDL_GetTicks();
	}
	CRandom(int nSeed) {
		m_nSeed	= nSeed;
	}
 
	int Get(int nMax) {
		m_nSeed	= m_nSeed * 0x343fD + 0x269ec3;
		return	((m_nSeed >> 16) & 0x7fff) % nMax;
	}
};
