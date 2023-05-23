//-*-C++-*-

#ifndef ANDROMEDA_STRUCTS_TOKENS_WORD_TOKEN_H_
#define ANDROMEDA_STRUCTS_TOKENS_WORD_TOKEN_H_

namespace andromeda
{

  class word_token: public base_types
  {
  public:

    /* The DEFAULT_POS != UNDEF_POS for a specific reason:
     * 
     * DEFAULT_POS is used to define the concept of a `word`-token (eg `report` can be a `noun` or `verb`)
     * UNDEF_POS is used to signal that the POS-tagger could not determine the POS of a word-token
     *
     * --> in order to distinguish in the GLM the node versus the word (i.e. hash is different between
     *     node-token and word-token), we need to distinguish between a default POS and an undefined POS.
     */

    const static inline std::vector<std::string> HEADERS = {"hash", "char_i", "char_j",
							    "word-index",
							    "pos", "tag", "known",
							    "word", "original"};

    /*
    typedef float    fval_type;
    typedef uint16_t flvr_type;    
    typedef uint64_t hash_type;
    
    typedef            uint64_t     index_type;
    typedef std::array<uint64_t, 2> range_type;
    typedef std::array<uint64_t, 2> coord_type;
    */
    
    const static inline std::string DEFAULT="__default__";
    
    const static inline std::string DEFAULT_WORD=DEFAULT;
    const static inline std::string DEFAULT_POS=DEFAULT;

    const static inline std::string UNDEF_POS="__undef__";
    const static inline std::string UNDEF_TAG="";

  public:

    word_token(std::string word);

    word_token(index_type i,
               std::string word);

    word_token(index_type i,
	       index_type j,
               std::string word);
    
    word_token(std::string word,
               std::string pos);

    word_token(index_type i,
               std::string word,
               std::string pos);

    word_token(index_type i, index_type j,
               std::string word, std::string pos,
	       std::set<std::string> tags);

    /*
    word_token(hash_type hash, index_type i, index_type j,
               std::string word, std::string pos,
	       std::set<std::string> tags);    
    */
    
    word_token(hash_type hash, index_type i, index_type j,
	       std::string pos, std::set<std::string> tags, bool known,
	       std::string word);
    
    bool has_default_pos() { return DEFAULT_POS==pos; }
    bool has_default_word() { return DEFAULT_WORD==word; }

    hash_type get_hash() const { return hash; }
    
    range_type get_rng() { return rng; };
    index_type get_rng(index_type l) { return rng[l]; };

    std::string get_word() const { return word; }
    std::string get_orig(const std::string& text) { return text.substr(rng[0], rng[1]-rng[0]); }

    std::string get_pos() { return pos; } // part-of-speech
    std::set<std::string> get_tags() const { return tags; } // tags

    void set_word(std::string word);    
    void set_pos(std::string pos);
    void set_tag(std::string tag);
    void set_known(bool known);

    bool has_tag(std::string tag);
    bool is_known();
      
    word_token get_word_token();
    word_token get_pos_token();

  private:

    void verify();

  private:

    hash_type hash;
    range_type rng;

    std::string word;
    std::string pos; // part-of-speech

    bool known; // not out-of-vocabulary
    
    std::set<std::string> tags;
  };

  word_token::word_token(std::string word):
    hash(utils::to_hash(word)),
    rng({0,word.size()}),

    word(word),
    pos(UNDEF_POS),
    known(false),    

    tags({})
  {
    verify();
  }

  word_token::word_token(index_type i,
                         std::string word):
    hash(utils::to_hash(word)),
    rng({i,i+word.size()}),

    word(word),
    pos(UNDEF_POS),
    known(false),
    
    tags({})
  {
    verify();
  }

  word_token::word_token(index_type i,
			 index_type j,
                         std::string word):
    hash(utils::to_hash(word)),
    rng({i,j}),

    word(word),
    pos(UNDEF_POS),
    known(false),    
    
    tags({})
  {
    verify();
  }  

  word_token::word_token(std::string word,
                         std::string pos):
    hash(utils::to_hash(word)),
    rng({0,word.size()}),

    word(word),
    pos(pos),
    known(false),    
    
    tags({})
  {
    verify();
  }

  word_token::word_token(index_type i,
                         std::string word,
                         std::string pos):
    hash(utils::to_hash(word)),
    rng({i,i+word.size()}),

    word(word),
    pos(pos),
    known(false),    

    tags({})
  {
    verify();
  }
  
  word_token::word_token(index_type i, index_type j,
                         std::string word, std::string pos,
			 std::set<std::string> tags):
    hash(utils::to_hash(word)),
    rng({i,j}),

    word(word),
    pos(pos),
    known(false),    

    tags(tags)
  {
    verify();
  }

  word_token::word_token(hash_type hash, index_type i, index_type j,
			 std::string pos, std::set<std::string> tags, bool known,
			 std::string word):
    hash(hash),
    rng({i,j}),
    
    word(word),
    pos(pos),    
    known(known),

    tags(tags)
  {
    verify();
  }    
  
  void word_token::verify()
  {
    if(word.size()>48)
      {
        word = DEFAULT_WORD;
        pos = UNDEF_POS;
      }

    if(pos=="" or pos=="NULL")
      {
	pos = UNDEF_POS;
      }
  }

  void word_token::set_word(std::string word)
  {
    this->word = word;
  }
  
  void word_token::set_pos(std::string pos)
  {
    this->pos = pos;
    verify();
  }

  void word_token::set_known(bool known)
  {
    this->known = known;
  }

  void word_token::set_tag(std::string tag)
  {
    this->tags.insert(tag);
  }

  bool word_token::has_tag(std::string tag)
  {
    return ((this->tags.count(tag))>0);
  }
  
  word_token word_token::get_word_token()
  {
    word_token token(rng[0], word, DEFAULT);
    return token;
  }

  word_token word_token::get_pos_token()
  {
    word_token token(rng[0], DEFAULT, pos);
    return token;
  }

  bool word_token::is_known()
  {
    return known;
  }
  
}

#endif
