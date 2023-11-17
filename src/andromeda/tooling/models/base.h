//-*-C++-*-

#ifndef ANDROMEDA_MODELS_BASE_MODEL_H_
#define ANDROMEDA_MODELS_BASE_MODEL_H_

namespace andromeda
{
  class base_nlp_model: public glm_variables
  {
  public:
    
    typedef typename word_token::index_type index_type;
    typedef typename word_token::range_type range_type;
    
  public:

    base_nlp_model() {};
    virtual ~base_nlp_model() {};

    virtual std::set<model_name> get_dependencies() = 0;

    virtual model_type get_type() = 0;
    virtual model_name get_name() = 0;

    virtual std::string get_key() { return to_key(this->get_name()); }

    /* INFERENCE */
    
    template<typename subject_type>
    bool satisfies_dependencies(subject_type& subj);

    template<typename subject_type>
    static bool satisfies_dependencies(subject_type& subj, const std::set<model_name>& deps);

    template<typename subject_type>
    bool update_applied_models(subject_type& subj);

    virtual bool match(std::string& text, nlohmann::json& annot) { return false; }
    virtual bool apply(std::string& text, nlohmann::json& annots) { return false; }

    virtual bool apply(subject<TEXT>& subj) = 0;// { return false; }
    //virtual bool apply(subject<TABLE>& subj) = 0;//{ return false; }

    virtual bool apply(subject<TABLE>& subj); //{ return false; }
    virtual bool apply_on_table_data(subject<TABLE>& subj) { return false; }
    
    virtual bool apply(subject<FIGURE>& subj);
    virtual bool apply(subject<DOCUMENT>& subj);

    static bool finalise(subject<DOCUMENT>& subj) { return false; }

    /* TRAIN */

    virtual bool is_trainable() { return false; }

    virtual nlohmann::json create_train_config() { return nlohmann::json::object({}); }

    virtual bool prepare_data_for_train(nlohmann::json args,
					std::vector<std::shared_ptr<base_nlp_model> >& dep_models) { return false; }

    virtual bool train(nlohmann::json args) { return false; }

    virtual bool evaluate_model(nlohmann::json args,
				std::vector<std::shared_ptr<base_nlp_model> >& dep_models) { return false; }
  };

  template<typename subject_type>
  bool base_nlp_model::satisfies_dependencies(subject_type& subj)
  {
    return satisfies_dependencies(subj, get_dependencies());
  }

  template<typename subject_type>
  bool base_nlp_model::satisfies_dependencies(subject_type& subj, const std::set<model_name>& deps)
  {
    bool result=true;
    for(auto dep:deps)
      {
        if(subj.applied_models.count(to_key(dep))==0)
          {
            result = false;
          }
      }

    return result;
  }
  
  template<typename subject_type>
  bool base_nlp_model::update_applied_models(subject_type& subj)
  {
    subj.applied_models.insert(this->get_key());
    return true;
  }

  bool base_nlp_model::apply(subject<TABLE>& subj)
  {
    //LOG_S(INFO) << __FUNCTION__ << " (apply on table)";
    
    if(not satisfies_dependencies(subj))
      {
        return false;
      }

    for(auto& caption:subj.captions)
      {
	//LOG_S(INFO) << __FUNCTION__ << " (apply on table-caption)";
        this->apply(*caption);
      }

    this->apply_on_table_data(subj);

    return true;
  }
  
  bool base_nlp_model::apply(subject<FIGURE>& subj)
  {
    //LOG_S(INFO) << __FUNCTION__ << " (apply on figure)";
    
    if(not satisfies_dependencies(subj))
      {
        return false;
      }

    for(auto& caption:subj.captions)
      {
	//LOG_S(INFO) << __FUNCTION__ << " (apply on figure-caption)";
        this->apply(*caption);
      }

    return true;
  }
  
  bool base_nlp_model::apply(subject<DOCUMENT>& subj)
  {
    if(not satisfies_dependencies(subj))
      {
        return false;
      }
    
    //subj.join_properties_with_texts();
    for(auto& text_ptr:subj.texts)
      {
        this->apply(*text_ptr);
      }
    //subj.clear_properties_from_texts();

    //subj.join_properties_with_tables();
    for(auto& table_ptr:subj.tables)
      {
        this->apply(*table_ptr);
      }
    //subj.clear_properties_from_tables();

    //subj.join_properties_with_tables();
    for(auto& figure_ptr:subj.figures)
      {
        this->apply(*figure_ptr);
      }
    //subj.clear_properties_from_tables();
    
    return update_applied_models(subj);
  }  

}

#endif
