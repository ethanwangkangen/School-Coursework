use std::collections::HashSet;
use std::sync::{LazyLock, Mutex};
use std::ptr::NonNull;


const MAX_USERS: usize = 1000;
const MAX_NAME_LEN: usize = 50;
const MAX_EMAIL_LEN: usize = 50;
const MAX_PASSWORD_LENGTH: usize = 100;
const INACTIVITY_THRESHOLD: i32 = 5;
const MAX_SESSION_TOKEN_LEN: usize = 32;


// Pointer registry?
// If pointer is in registry, it means its in use.
// Remove before freeing.
// If pointer not in registry, don't access!
static REGISTRY: LazyLock<Mutex<HashSet<usize>>> =
    LazyLock::new(|| Mutex::new(HashSet::new()));

#[no_mangle]
pub extern "C" fn registry_add(ptr: *mut UserStruct) {
    if let Some(nn) = NonNull::new(ptr) {
        let mut reg = REGISTRY.lock().unwrap();
        reg.insert(nn.as_ptr() as usize);
    }
}

#[no_mangle]
pub extern "C" fn registry_remove(ptr: *mut UserStruct) {
    if let Some(nn) = NonNull::new(ptr) {
      //  println!("Removing from registry");
        let mut reg = REGISTRY.lock().unwrap();
        let present = reg.remove(&(nn.as_ptr() as usize));
    }
}

#[no_mangle]
pub extern "C" fn registry_is_alive(ptr: *mut UserStruct) -> i32 {
    if let Some(nn) = NonNull::new(ptr) {
        let reg = REGISTRY.lock().unwrap();
        reg.contains(&(nn.as_ptr() as usize)) as i32
    } else {
        0
    }
}

#[repr(C)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum OwnershipType {
    C = 0,
    Rust = 1,
}

#[derive(Debug, Clone)]
#[repr(C)]
pub struct UserStruct {
    pub password: [u8; MAX_PASSWORD_LENGTH],
    pub username: [u8; MAX_NAME_LEN],
    pub user_id: i32,
    pub email: [u8; MAX_EMAIL_LEN],
    pub inactivity_count: i32,
    pub is_active: i32,
    pub session_token: [u8; MAX_SESSION_TOKEN_LEN],
    pub shared: i32,
    pub owner: OwnershipType,

}

impl Default for UserStruct {
    fn default() -> Self {
        UserStruct {
            password: [0; MAX_PASSWORD_LENGTH],
            user_id: 0,
            email: [0; MAX_EMAIL_LEN],
            inactivity_count: 0,
            username: [0; MAX_NAME_LEN],
            session_token: [0; MAX_SESSION_TOKEN_LEN],
            is_active: 0,
            shared: 0,
            owner: OwnershipType::Rust,
        }
    }
}

#[derive(Debug)]
pub struct UserDatabase {
    pub users: [Option<Box<UserStruct>>; MAX_USERS],
    pub count: i32,
    pub capacity: i32,
}

use std::str;
use std::cmp::min;

// Helper function.
// Takes in a mutable reference to an array, and returns a String
pub fn array_to_string<const N: usize>(bytes: &[u8; N]) -> String {
    let len = bytes.iter().position(|&b| b == 0).unwrap_or(N);
    String::from_utf8_lossy(&bytes[..len]).to_string()
}

pub fn init_database() -> Box<UserDatabase> {
  let db = UserDatabase {
    users: std::array::from_fn(|_| None),
    count: 0,
    capacity: MAX_USERS as i32,
  };

  Box::new(db)
}


pub fn add_user(db: &mut UserDatabase, user: Box<UserStruct>) {
    if (db.count as usize) < MAX_USERS {
        let ptr = Box::into_raw(user);   // take ownership
        unsafe {
            registry_add(ptr);           // mark as alive
            // rewrap so Rust still manages it
            db.users[db.count as usize] = Some(Box::from_raw(ptr));
            // this is actually ok even if it's a C pointer? 
            // just don't set db.users[i] to None unless sure it's ok. Check registry.
        }
        db.count += 1;
    }
}


fn free_user(user : &mut UserStruct){
}


pub fn print_database(db: &UserDatabase) {
 for i in 0..db.count {
    // 'if let' is a pattern matching expression
    //
    // Here, it tries to match &db.users[i] (a reference to an Option<Box<UserStruct>>)
    // against the pattern Some(user).
    //
    // If the slot contains Some, the block runs and'user' is a reference to the Box<UserStruct>.
    // If it's None, the block is skipped.

    if let Some(user) = &db.users[i as usize] {
        let ptr: *mut UserStruct = &**user as *const UserStruct as *mut UserStruct;

        if registry_is_alive(ptr)==0 {
            continue;
        }

        println!(
            "User: {}, ID: {}, Email: {}, Inactivity: {}, Password: {}",
            array_to_string(&user.username),
            user.user_id,
            array_to_string(&user.email),
            user.inactivity_count,
            array_to_string(&user.password),
        );
    }
  }
}


fn copy_string(dest : &mut [u8], src : &str, n : usize) {
    let bytes = src.as_bytes();

    let max = dest.len() -1;
    let copyLen = min(n, min(bytes.len(), max));

    for i in 0..copyLen {
        dest[i] = bytes[i];
    }
    dest[copyLen] = 0; //null terminate
}

pub fn create_user(username: &str, email: &str, user_id: i32, password: &str) -> Box<UserStruct> {
    let mut user = UserStruct {
        password: [0; MAX_PASSWORD_LENGTH],
        username: [0; MAX_NAME_LEN],
        user_id,
        email: [0; MAX_EMAIL_LEN],
        inactivity_count: 0,
        is_active : 0,
        session_token: [0; MAX_SESSION_TOKEN_LEN],
        shared:0,
        owner:OwnershipType::Rust,
    };

    copy_string(&mut user.password, password, password.len());
    copy_string(&mut user.username, username, username.len());
    copy_string(&mut user.email, email, email.len());
    Box::new(user) // Return the user
}

pub fn find_user_by_id(db: &mut UserDatabase, user_id: i32) -> Option<&mut UserStruct> {
    // .iter_mut() to bypass multiple borrowing issues when iterating via for i in 0..db.count
    for user_option in db.users.iter_mut() {
        // db.users is [Option<Box<UserStruct>>; MAX_USERS]
        // iter_mut() produces an iterator over this

        // user_option has type: &mut Option<Box<UserStruct>>
        if let Some(user) = user_option {
            // user is now &mut <Box<UserStruct>>

            if user.user_id == user_id {
                
                // First dereference: get Box<UserStruct>
                // Second dereference: get UserStruct
                // Add in &mut: get &mut UserStruct
                // Add in Some(): get Option<&mut UserStruct>
                return Some(&mut **user); 

            }
        }
    }
    None
}

pub fn cleanup_database(db : &mut UserDatabase) {
    for i in 0..db.count {
        if let Some(user) = db.users[i as usize].take() {
            if user.owner == OwnershipType::C {
                // put it back, don’t free
                db.users[i as usize] = Some(user);
                continue;
            }
            // edit registry
            let ptr = Box::into_raw(user);
            unsafe { registry_remove(ptr); }
            unsafe { Box::from_raw(ptr); } // to free? necessary?
            db.users[i as usize] = None;
        }

    }
}

pub fn update_database_daily(db: &mut UserDatabase) {
    
    for i in 0..db.count {
        if let Some(user) = db.users[i as usize].take() {
            // get raw pointer for registry check
            let ptr: *mut UserStruct = &*user as *const UserStruct as *mut UserStruct;

            if unsafe { registry_is_alive(ptr)==0 } {
                db.users[i as usize] = Some(user); // dead, just put it back
                continue;
            }

            if user.owner == OwnershipType::C {
                db.users[i as usize] = Some(user); // not owned, just put it back
                continue;
            }

            // Rust-owned users: just free.
            if user.inactivity_count > INACTIVITY_THRESHOLD && user.is_active==0{
                println!(
                    "[Rust] Removing user {} for inactivity",
                    array_to_string(&user.username)
                );

                // Remove from registry and drop the box
                let raw = Box::into_raw(user);
                unsafe { registry_remove(raw) };
                unsafe { drop(Box::from_raw(raw)); }

                db.users[i as usize] = None;
                //db.count -= 1;
            }
            else {

                let mut user = user;
                user.inactivity_count += 1;
                db.users[i as usize] = Some(user);
            }

        }
    }
}



pub fn update_username (db: &mut UserDatabase, username : &str, new_username : &str) {
    if let Some(user) = find_user_by_username_mut(db, username) {
        // user is &mut UserStruct

        if new_username.len() >= 50 {
            let truncated: String = new_username.chars().take(49).collect();
            copy_string(&mut user.username, &truncated, truncated.len());
        } else {
           copy_string(&mut user.username, new_username, new_username.len());
        }
    }
}

pub fn user_login(db: &mut UserDatabase, username: &str) {
    if let Some(user) = find_user_by_username_mut(db, username) {
        //user is &mut UserStruct
        user.inactivity_count = 0;
    }
}

pub fn get_password(db : &mut UserDatabase, username: &str) -> Option<String> {
    if let Some(user) = find_user_by_username_mut(db, username) {
        //user is &mut UserStruct
        Some(array_to_string(&user.password))
    } else {
        None
    }
}

fn update_password(db : &mut UserDatabase, username: &str , password: &str) {
    if let Some(user) = find_user_by_username_mut(db, username) {
        //user is &mut UserStruct
        copy_string(&mut user.password, password, password.len());
    }  
}

fn print_user(db : &mut UserDatabase, username : &str) {
    if let Some(user) = find_user_by_username_mut(db, username) {
        //user is a &mut UserStruct
        println!("User[{}] {}: Email: {}, Inactivity: {}, Password: {} \n",
                 user.user_id,
                 array_to_string(&user.username),
                 array_to_string(&user.email),
                 user.inactivity_count,
                 array_to_string(&user.password)
                 );
    } 

}

pub fn find_user_by_username<'a>(db : & 'a UserDatabase, username: &'a str) -> Option<&'a UserStruct> {
    for user_option in db.users.iter() {
        if let Some(user) = user_option {
            let raw_ptr: *mut UserStruct = &**user as *const UserStruct as *mut UserStruct;
            if registry_is_alive(raw_ptr) ==0 {
                continue;
            }
            if std::str::from_utf8(&user.username)
                .unwrap_or("")
                .trim_end_matches(char::from(0)) == username
                {
                    return Some(& **user);
                }
        }
    } 
    None

}

pub fn find_user_by_username_mut<'a>(db: & 'a mut UserDatabase, username: &'a str) -> Option<&'a mut UserStruct> {
    for user_option in db.users.iter_mut() {
        if let Some(user) = user_option{
            if registry_is_alive(&mut **user) ==0{
                continue;
            }
            if std::str::from_utf8(&user.username)
                .unwrap_or("")
                .trim_end_matches(char::from(0)) == username
                {
                    return Some(&mut **user);
                }
        }
    } None
}




fn main() {
    print!("Hello, world!");
}
