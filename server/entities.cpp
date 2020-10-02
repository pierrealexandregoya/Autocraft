#include "entities.hpp"

#include "config.hpp"
#include "entity.hpp"
//#include "npc.hpp"
//#include "player.hpp"
//#include "stationnary_entity.hpp"

namespace autocraft {

template class AUTOCRAFT_API entity_container<entity>;
//template class AUTOCRAFT_API entity_container<located_entity>;
//template class AUTOCRAFT_API entity_container<stationnary_entity>;
//template class AUTOCRAFT_API entity_container<mobile_entity>;
//template class AUTOCRAFT_API entity_container<npc>;
//template class AUTOCRAFT_API entity_container<player>;

//template AUTOCRAFT_API entity_container<entity>::operator entity_container<located_entity>;

} // namespace autocraft
