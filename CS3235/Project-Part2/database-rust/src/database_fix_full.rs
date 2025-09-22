const MAX_USERS: usize = 100;
const MAX_NAME_LEN: usize = 50;
const MAX_EMAIL_LEN: usize = 50;
const MAX_PASSWORD_LENGTH: usize = 100;
const INACTIVITY_THRESHOLD: i32 = 5;
const MAX_SESSION_TOKEN_LEN: usize = 32;

#[derive(Debug, Clone,Eq, PartialEq)]
#[repr(C)]
pub enum OwnershipType {
    RustOwned,
    COwned,
}
#[derive(Debug,Clone)]
#[repr(C)]
pub struct UserStruct {
    pub password: [u8; MAX_PASSWORD_LENGTH],
    pub username: [u8; MAX_NAME_LEN],
    pub user_id: i32,
    pub email: [u8; MAX_EMAIL_LEN],
    pub inactivity_count: i32,
    pub is_active: i32,
    pub session_token: [u8; MAX_SESSION_TOKEN_LEN],
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
            owner:OwnershipType::RustOwned,
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
//pub fn array_to_string<const N: usize>(bytes: &[u8; N]) -> String {
//    match str::from_utf8(bytes) {
//        Ok(s) => s.trim_end_matches('\x00').to_string(),
//        Err(e) => format!("Error: {}", e),
//    }
//}
pub fn array_to_string<const N: usize>(bytes: &[u8; N]) -> String {
    let end = bytes.iter().position(|&b| b == 0).unwrap_or(N);
    String::from_utf8_lossy(&bytes[..end]).to_string()
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
  let mut user_mut : Box<UserStruct> = user;
  if (db.count as usize)< MAX_USERS as usize{
    

    user_mut.user_id = db.count;
    db.users[db.count as usize] = Some(user_mut);
    db.count +=1;
  }
}


fn free_user(user : &mut UserStruct){
    if user.owner == OwnershipType::RustOwned{
    
        user.is_active=0;
    }
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
        is_active : 1,
        session_token: [0; MAX_SESSION_TOKEN_LEN],
        owner: OwnershipType::RustOwned,
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
        if let Some(user) = &mut db.users[i as usize]{
            if user.owner == OwnershipType::RustOwned{
                db.users[i as usize] = None;
            }
        }
    }
}

pub fn update_database_daily(db: &mut UserDatabase) {
    for i in 0..db.count {
        if let Some(user) = &mut db.users[i as usize] { //mutable borrow
            if user.inactivity_count > INACTIVITY_THRESHOLD && user.owner == OwnershipType::RustOwned{

                db.users[i as usize] = None;
                //free_user(user);
            } else {
                user.inactivity_count +=1 ;
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
